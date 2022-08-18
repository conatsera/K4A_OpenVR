import 'dart:math';
import 'dart:ffi';

import 'package:flutter/material.dart';
import 'package:vector_math/vector_math_64.dart';

import 'services.dart';
import 'generated/calibration.pbgrpc.dart';

import 'scaffolding.dart';
import 'attitude.dart';

final DynamicLibrary calibratorLib = DynamicLibrary.open("calibrator.dll");

final bool Function() initOpenvr = calibratorLib
    .lookup<NativeFunction<Bool Function()>>('init_openvr')
    .asFunction();

final void Function() shutdownOpenvr = calibratorLib
    .lookup<NativeFunction<Void Function()>>('shutdown_openvr')
    .asFunction();

class HmdMatrix34_t extends Struct {
  @Array(3, 4)
  external Array<Array<Float>> m;
}

class CalibrationPoint extends Struct {
  external HmdMatrix34_t hmd;
  external HmdMatrix34_t head;
}

final HmdMatrix34_t Function() getOpenvrHmdPose = calibratorLib
    .lookup<NativeFunction<HmdMatrix34_t Function()>>('get_openvr_hmd_pose')
    .asFunction();

final HmdMatrix34_t Function() getOpenvrK4aHeadPose = calibratorLib
    .lookup<NativeFunction<HmdMatrix34_t Function()>>(
        'get_openvr_k4a_head_pose')
    .asFunction();

final void Function() recordCalibrationPoint = calibratorLib
    .lookup<NativeFunction<Void Function()>>('record_calibration_point')
    .asFunction();

final void Function() resetCalibrationPoints = calibratorLib
    .lookup<NativeFunction<Void Function()>>('reset_calibration_points')
    .asFunction();

final CalibrationPoint Function(int) getCalibrationPoint = calibratorLib
    .lookup<NativeFunction<CalibrationPoint Function(Uint16)>>(
        'get_calibration_point')
    .asFunction();

final HmdMatrix34_t Function() solveTransformMatrix = calibratorLib
    .lookup<NativeFunction<HmdMatrix34_t Function()>>('solve_transform_matrix')
    .asFunction();

bool _isOpenvrInit = false;

class CalibratorPage extends StatefulWidget {
  const CalibratorPage({Key? key}) : super(key: key);

  @override
  State<CalibratorPage> createState() => _CalibratorPageState();
}

Vector3 eulerAngles(Quaternion quat) {
  final y = 2 * (quat.y * quat.z + quat.w * quat.x);
  final x =
      quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z;

  double pitch = 0.0;

  if (x == 0.0 && y == 0.0) {
    pitch = 2.0 * atan2(quat.x, quat.w);
  } else {
    pitch = atan2(y, x);
  }

  double t = -2 * (quat.x * quat.z - quat.w * quat.y);
  if (t > 1.0) {
    t = 1.0;
  } else if (t < -1.0) {
    t = -1.0;
  }

  final yaw = asin(t);

  final roll = atan2(2.0 * (quat.x * quat.y + quat.w * quat.z),
      quat.w * quat.w + quat.x * quat.x - quat.y * quat.y - quat.z * quat.z);
  return Vector3(pitch, yaw, roll);
}

class _CalibratorPageState extends State<CalibratorPage> {
  @override
  void dispose() {
    if (_isOpenvrInit) {
      shutdownOpenvr();
    }
    super.dispose();
  }

  void _openSettings() {
    Navigator.popAndPushNamed(context, '/home');
  }

  final Vector3 _kinectOrientation = Vector3.zero();

  final Vector3 _standingHmdPosition = Vector3.zero();
  final Quaternion _standingHmdOrientation = Quaternion.identity();
  final Vector3 _standingHmdEulerAngles = Vector3.zero();

  final Vector3 _hmdPosition = Vector3.zero();
  final Quaternion _hmdOrientation = Quaternion.identity();
  final Vector3 _hmdEulerAngles = Vector3.zero();

  final Vector3 _standingHeadPosition = Vector3.zero();
  final Quaternion _standingHeadOrientation = Quaternion.identity();
  final Vector3 _standingHeadEulerAngles = Vector3.zero();

  final Vector3 _headPosition = Vector3.zero();
  final Quaternion _headOrientation = Quaternion.identity();
  final Vector3 _headEulerAngles = Vector3.zero();

  final Vector3 _diffPos = Vector3.zero();
  final Quaternion _diffQuat = Quaternion.identity();
  final Vector3 _diffAngles = Vector3.zero();

  Future<void> startPoseStream() async {
    if (!_isOpenvrInit) {
      _isOpenvrInit = initOpenvr();
    }
    if (_isOpenvrInit) {
      final poseStream =
          calibrationClient.getPoseStream(CalibrationDataRequest());
      poseStream.handleError((e) => handleGrpcError(context, e));
      await for (var r in poseStream) {
        final standingHmdPose = getOpenvrHmdPose();
        final standingHmdMat = Matrix4(
            standingHmdPose.m[0][0],
            standingHmdPose.m[0][1],
            standingHmdPose.m[0][2],
            standingHmdPose.m[0][3],
            standingHmdPose.m[1][0],
            standingHmdPose.m[1][1],
            standingHmdPose.m[1][2],
            standingHmdPose.m[1][3],
            standingHmdPose.m[2][0],
            standingHmdPose.m[2][1],
            standingHmdPose.m[2][2],
            standingHmdPose.m[2][3],
            0.0,
            0.0,
            0.0,
            1.0);

        final Quaternion standingHmdQuat =
            Quaternion.fromRotation(standingHmdMat.getRotation());

        final standingHmdEulers = eulerAngles(standingHmdQuat);

        final hmdMat = Matrix4(
            r.hmd.pose.m00,
            r.hmd.pose.m01,
            r.hmd.pose.m02,
            r.hmd.pose.m03,
            r.hmd.pose.m10,
            r.hmd.pose.m11,
            r.hmd.pose.m12,
            r.hmd.pose.m13,
            r.hmd.pose.m20,
            r.hmd.pose.m21,
            r.hmd.pose.m22,
            r.hmd.pose.m23,
            0.0,
            0.0,
            0.0,
            1.0);

        hmdMat.translate(-r.hmd.pose.m03, -r.hmd.pose.m13, -r.hmd.pose.m23);

        final Quaternion hmdQuat =
            Quaternion.fromRotation(hmdMat.getRotation());

        final hmdEulers = eulerAngles(hmdQuat);

        final standingHeadPose = getOpenvrK4aHeadPose();
        final standingHeadMat = Matrix4(
            standingHeadPose.m[0][0],
            standingHeadPose.m[0][1],
            standingHeadPose.m[0][2],
            standingHeadPose.m[0][3],
            standingHeadPose.m[1][0],
            standingHeadPose.m[1][1],
            standingHeadPose.m[1][2],
            standingHeadPose.m[1][3],
            standingHeadPose.m[2][0],
            standingHeadPose.m[2][1],
            standingHeadPose.m[2][2],
            standingHeadPose.m[2][3],
            0.0,
            0.0,
            0.0,
            1.0);

        final Quaternion standingHeadQuat =
            Quaternion.fromRotation(standingHeadMat.getRotation());

        final standingHeadEulers = eulerAngles(standingHeadQuat);

        final Quaternion headQuat = Quaternion(r.head.pose.rot.x,
            r.head.pose.rot.y, r.head.pose.rot.z, r.head.pose.rot.w);

        final headEulers = eulerAngles(headQuat);
        setState(() {
          _standingHmdPosition.setFrom(standingHmdMat.getTranslation());
          _standingHmdOrientation.setFrom(standingHmdQuat);
          _standingHmdEulerAngles.setFrom(standingHmdEulers);

          _hmdPosition.x = r.hmd.pose.m03;
          _hmdPosition.y = r.hmd.pose.m13;
          _hmdPosition.z = r.hmd.pose.m23;
          _hmdOrientation.setFrom(hmdQuat);

          _hmdEulerAngles.setFrom(hmdEulers);

          _standingHeadPosition.setFrom(standingHeadMat.getTranslation());
          _standingHeadOrientation.setFrom(standingHeadQuat);
          _standingHeadEulerAngles.setFrom(standingHeadEulers);

          _headPosition.x = r.head.pose.pos.x;
          _headPosition.y = r.head.pose.pos.y;
          _headPosition.z = r.head.pose.pos.z;
          _headOrientation.setFrom(headQuat);

          _headEulerAngles.setFrom(headEulers);
        });
      }
    }
  }

  void _openCalibrator() {
    Navigator.pop(context);
  }

  void _updateCalibration() {
    calibrationClient.updateCalibration(UpdateCalibrationRequest(
        qWorldFromDriverRotation: Quat(
          w: _diffQuat.w,
          x: _diffQuat.x,
          y: _diffQuat.y,
          z: _diffQuat.z,
        ),
        vecWorldFromDriverTranslation:
            Vec3(x: _diffPos.x, y: _diffPos.y, z: _diffPos.z)));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      drawer: Drawer(
        child: ListView(
          children: [
            ListTile(
              title: const Text('Settings'),
              onTap: _openSettings,
            ),
            ListTile(
              title: const Text('Calibrator'),
              onTap: _openCalibrator,
            ),
          ],
        ),
      ),
      appBar: AppBar(
        title: const Text('Calibrator'),
      ),
      body: Center(
        child: Column(mainAxisAlignment: MainAxisAlignment.center, children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Column(mainAxisAlignment: MainAxisAlignment.center, children: [
                AttitudeIndicator(
                    yaw: _hmdEulerAngles.x,
                    pitch: _hmdEulerAngles.y,
                    roll: _hmdEulerAngles.z),
                AttitudeIndicator(
                    yaw: _headEulerAngles.x,
                    pitch: _headEulerAngles.y,
                    roll: _headEulerAngles.z),
              ]),
              Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  AttitudeIndicator(
                      yaw: _standingHmdEulerAngles.x,
                      pitch: _standingHmdEulerAngles.y,
                      roll: _standingHmdEulerAngles.z),
                  AttitudeIndicator(
                      yaw: _standingHeadEulerAngles.x,
                      pitch: _standingHeadEulerAngles.y,
                      roll: _standingHeadEulerAngles.z),
                ],
              )
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              IconButton(
                onPressed: () async {
                  await startPoseStream();
                },
                icon: const Icon(Icons.start),
              ),
              IconButton(
                icon: const Icon(Icons.stop),
                onPressed: () {
                  calibrationClient
                      .stopPoseStream(CalibrationDataRequest())
                      .catchError((e) => handleGrpcError(context, e));
                },
              ),
              IconButton(
                onPressed: () {
                  setState(() {
                    _diffQuat.setFrom(Quaternion.identity());
                    _diffPos.setZero();
                  });
                  _updateCalibration();
                },
                icon: const Icon(Icons.restore),
              ),
            ],
          )
        ]),
      ),
      bottomNavigationBar: const BottomAppBarControls(),
      floatingActionButton: FloatingActionButton(
        child: const Icon(Icons.compass_calibration),
        onPressed: () {},
      ),
    );
  }
}

/*
calibrationClient
              .getCalibrationData(CalibrationDataRequest())
              .then((r) {
            final Vector3 left = Vector3(1, 0, 0);
            final Vector3 forward = Vector3(0, -1, 0);
            final Vector3 up = Vector3(0, 0, -1);

            final Vector3 accelNorm =
                Vector3(r.acc.sample.x, r.acc.sample.y, r.acc.sample.z)
                    .normalized();

            final angleX = radians2Degrees * left.dot(accelNorm);
            final angleY = radians2Degrees * forward.dot(accelNorm);
            final angleZ = radians2Degrees * up.dot(accelNorm);

            final hmdMat = Matrix3(
              r.data.hmd.pose.m00,
              r.data.hmd.pose.m01,
              r.data.hmd.pose.m02,
              r.data.hmd.pose.m10,
              r.data.hmd.pose.m11,
              r.data.hmd.pose.m12,
              r.data.hmd.pose.m20,
              r.data.hmd.pose.m21,
              r.data.hmd.pose.m22,
            );

            final Quaternion hmdQuat =
                Quaternion.fromRotation(hmdMat).normalized();

            final hmdEulers = eulerAngles(hmdQuat);

            final standingHmdPose = getOpenvrHmdPose();

            final standingHmdRotMat = Matrix3(
              standingHmdPose.m[0][0],
              standingHmdPose.m[0][1],
              standingHmdPose.m[0][2],
              standingHmdPose.m[1][0],
              standingHmdPose.m[1][1],
              standingHmdPose.m[1][2],
              standingHmdPose.m[2][0],
              standingHmdPose.m[2][1],
              standingHmdPose.m[2][2],
            );

            final standingHmdPos = Vector3(standingHmdPose.m[0][3],
                standingHmdPose.m[1][3], standingHmdPose.m[2][3]);

            final Quaternion standingHmdQuat =
                Quaternion.fromRotation(standingHmdRotMat);

            final standingHmdMat = Matrix4.zero()
              ..setFromTranslationRotation(standingHmdPos, standingHmdQuat);

            final standingHmdEulers = eulerAngles(standingHmdQuat);

            final Quaternion headQuat = Quaternion(
              r.data.head.pose.rot.x,
              r.data.head.pose.rot.y,
              r.data.head.pose.rot.z,
              r.data.head.pose.rot.w,
            ).normalized();

            final Vector3 headPos = Vector3(
              r.data.head.pose.pos.x,
              r.data.head.pose.pos.y,
              r.data.head.pose.pos.z,
            );

            final Matrix4 headMat = Matrix4.compose(
              headPos,
              headQuat,
              Vector3.all(1.0),
            );

            final headEulers = eulerAngles(headQuat);

            final diffQuat = standingHmdQuat * headQuat.conjugated();
            final diffAngles = eulerAngles(diffQuat);

            final diffTranslation = standingHmdMat.getTranslation() -
                Vector3(
                  r.data.head.pose.pos.x,
                  r.data.head.pose.pos.y,
                  r.data.head.pose.pos.z,
                );

            setState(() {
              _kinectOrientation.x = angleX;
              _kinectOrientation.y = angleY;
              _kinectOrientation.z = angleZ;

              _hmdPosition.x = r.data.hmd.pose.m03;
              _hmdPosition.y = r.data.hmd.pose.m13;
              _hmdPosition.z = r.data.hmd.pose.m23;
              _hmdOrientation.w = hmdQuat.w;
              _hmdOrientation.x = hmdQuat.x;
              _hmdOrientation.y = hmdQuat.y;
              _hmdOrientation.z = hmdQuat.z;

              _hmdEulerAngles.x = hmdEulers.x;
              _hmdEulerAngles.y = hmdEulers.y;
              _hmdEulerAngles.z = hmdEulers.z;

              _headPosition.x = r.data.head.pose.pos.x;
              _headPosition.y = r.data.head.pose.pos.y;
              _headPosition.z = r.data.head.pose.pos.z;
              _headOrientation.w = headQuat.w;
              _headOrientation.x = headQuat.x;
              _headOrientation.y = headQuat.y;
              _headOrientation.z = headQuat.z;

              _headEulerAngles.x = headEulers.x;
              _headEulerAngles.y = headEulers.y;
              _headEulerAngles.z = headEulers.z;

              _diffPos.setFrom(diffTranslation);
              _diffQuat.setFrom(diffQuat);
              _diffAngles.setFrom(diffAngles);
            });

            calibrationClient.updateCalibration(UpdateCalibrationRequest(
                qWorldFromDriverRotation: Quat(
                  w: diffQuat.w,
                  x: diffQuat.x,
                  y: diffQuat.y,
                  z: diffQuat.z,
                ),
                vecWorldFromDriverTranslation: Vec3(
                    x: diffTranslation.x,
                    y: diffTranslation.y,
                    z: diffTranslation.z)));
          }).catchError((e) => handleGrpcError(context, e));

*/