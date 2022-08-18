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

class HmdMatrix34 extends Struct {
  @Array(3, 4)
  external Array<Array<Float>> m;
}

class CalibrationPoint extends Struct {
  external HmdMatrix34 hmd;
  external HmdMatrix34 head;
}

final HmdMatrix34 Function() getOpenvrHmdPose = calibratorLib
    .lookup<NativeFunction<HmdMatrix34 Function()>>('get_openvr_hmd_pose')
    .asFunction();

final HmdMatrix34 Function() getOpenvrK4aHeadPose = calibratorLib
    .lookup<NativeFunction<HmdMatrix34 Function()>>('get_openvr_k4a_head_pose')
    .asFunction();

final CalibrationPoint Function() recordCalibrationPoint = calibratorLib
    .lookup<NativeFunction<CalibrationPoint Function()>>(
        'record_calibration_point')
    .asFunction();

final void Function() resetCalibrationPoints = calibratorLib
    .lookup<NativeFunction<Void Function()>>('reset_calibration_points')
    .asFunction();

final int Function() getCalibrationPointCount = calibratorLib
    .lookup<NativeFunction<Uint64 Function()>>('get_calibration_point_count')
    .asFunction();

final CalibrationPoint Function(int) getCalibrationPoint = calibratorLib
    .lookup<NativeFunction<CalibrationPoint Function(Uint16)>>(
        'get_calibration_point')
    .asFunction();

final HmdMatrix34 Function() solveCalibrationMatrix = calibratorLib
    .lookup<NativeFunction<HmdMatrix34 Function()>>('solve_calibration_matrix')
    .asFunction();

final HmdMatrix34 Function() getCalibrationMatrix = calibratorLib
    .lookup<NativeFunction<HmdMatrix34 Function()>>('get_calibration_matrix')
    .asFunction();

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

class CalibratorPage extends StatefulWidget {
  const CalibratorPage({Key? key}) : super(key: key);

  @override
  State<CalibratorPage> createState() => _CalibratorPageState();
}

class _CalibratorPageState extends State<CalibratorPage> {
  bool _isOpenvrInit = false;
  late List<CalibrationPoint> _calibrationPoints;

  late HmdMatrix34 _calibrationMatrix;

  double _yaw = 0.0;
  double _pitch = 0.0;
  double _roll = 0.0;

  final Vector3 _diffPos = Vector3.zero();
  final Quaternion _diffQuat = Quaternion.identity();

  @override
  void initState() {
    super.initState();
    _isOpenvrInit = initOpenvr();

    _calibrationPoints = [];

    final pointCount = getCalibrationPointCount();
    for (int i = 0; i < pointCount; i++) {
      _calibrationPoints.add(getCalibrationPoint(i));
    }

    _calibrationMatrix = getCalibrationMatrix();
  }

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

  void _openCalibrator() {
    Navigator.pop(context);
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
      body: Column(
        children: [
          DataTable(
            columns: const [
              DataColumn(label: Text('HMD Position')),
              DataColumn(label: Text('Head Position')),
              DataColumn(label: Text('HMD Rotation')),
              DataColumn(label: Text('Head Rotation')),
              DataColumn(label: Text('Position Error')),
              DataColumn(label: Text('Rotation Error')),
            ],
            rows: _calibrationPoints.map<DataRow>((point) {
              final hmdPosition = Vector3(
                  point.hmd.m[0][3], point.hmd.m[1][3], point.hmd.m[2][3]);

              final hmdRotation = Quaternion.fromRotation(Matrix3(
                point.hmd.m[0][0],
                point.hmd.m[0][1],
                point.hmd.m[0][2],
                point.hmd.m[1][0],
                point.hmd.m[1][1],
                point.hmd.m[1][2],
                point.hmd.m[2][0],
                point.hmd.m[2][1],
                point.hmd.m[2][2],
              )).normalized();

              final hmdEulers = eulerAngles(hmdRotation);

              final headPosition = Vector3(
                  point.head.m[0][3], point.head.m[1][3], point.head.m[2][3]);

              final headRotation = Quaternion.fromRotation(Matrix3(
                point.head.m[0][0],
                point.head.m[0][1],
                point.head.m[0][2],
                point.head.m[1][0],
                point.head.m[1][1],
                point.head.m[1][2],
                point.head.m[2][0],
                point.head.m[2][1],
                point.head.m[2][2],
              )).normalized();

              final headEulers = eulerAngles(headRotation);

              final calibrationTransform = Vector3(_calibrationMatrix.m[0][3],
                  _calibrationMatrix.m[1][3], _calibrationMatrix.m[2][3]);

              final calibrationRotation = Quaternion.fromRotation(Matrix3(
                _calibrationMatrix.m[0][0],
                _calibrationMatrix.m[0][1],
                _calibrationMatrix.m[0][2],
                _calibrationMatrix.m[1][0],
                _calibrationMatrix.m[1][1],
                _calibrationMatrix.m[1][2],
                _calibrationMatrix.m[2][0],
                _calibrationMatrix.m[2][1],
                _calibrationMatrix.m[2][2],
              ));

              final positionError = (headPosition + calibrationTransform)
                  .absoluteError(hmdPosition);
              final rotationError = (headRotation * calibrationRotation)
                  .absoluteError(hmdRotation);

              return DataRow(
                cells: <DataCell>[
                  DataCell(
                    Text(
                        '[${hmdPosition.x.toStringAsPrecision(4)}, ${hmdPosition.y.toStringAsPrecision(4)}, ${hmdPosition.z.toStringAsPrecision(4)}]'),
                  ),
                  DataCell(
                    Text(
                        '[${headPosition.x.toStringAsPrecision(4)}, ${headPosition.y.toStringAsPrecision(4)}, ${headPosition.z.toStringAsPrecision(4)}]'),
                  ),
                  DataCell(
                    Text(
                        '[${hmdEulers.x.toStringAsPrecision(4)}, ${hmdEulers.y.toStringAsPrecision(4)}, ${hmdEulers.z.toStringAsPrecision(4)}]'),
                  ),
                  DataCell(
                    Text(
                        '[${headEulers.x.toStringAsPrecision(4)}, ${headEulers.y.toStringAsPrecision(4)}, ${headEulers.z.toStringAsPrecision(4)}]'),
                  ),
                  DataCell(Text('$positionError')),
                  DataCell(Text('$rotationError')),
                ],
              );
            }).toList(),
          ),
          ElevatedButton(
              onPressed: () {
                setState(() {
                  _calibrationPoints.add(recordCalibrationPoint());
                  /*for (var p in _calibrationPoints) {
                    print(Matrix3(
                      p.hmd.m[0][0],
                      p.hmd.m[0][1],
                      p.hmd.m[0][2],
                      p.hmd.m[1][0],
                      p.hmd.m[1][1],
                      p.hmd.m[1][2],
                      p.hmd.m[2][0],
                      p.hmd.m[2][1],
                      p.hmd.m[2][2],
                    ));
                    print(Matrix3(
                      p.head.m[0][0],
                      p.head.m[0][1],
                      p.head.m[0][2],
                      p.head.m[1][0],
                      p.head.m[1][1],
                      p.head.m[1][2],
                      p.head.m[2][0],
                      p.head.m[2][1],
                      p.head.m[2][2],
                    ));
                  }*/
                });
              },
              child: const Text('Add point')),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'Yaw:  ',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _yaw = value * degrees2Radians;
                              _diffQuat.setEuler(_yaw, _pitch, _roll);
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _yaw * radians2Degrees,
                        min: -180.0,
                        max: 180.0,
                        divisions: 1440,
                        label: ((_yaw * radians2Degrees).toString()),
                      );
                    }),
              ),
            ),
          ]),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'Pitch:',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _pitch = value * degrees2Radians;
                              _diffQuat.setEuler(_yaw, _pitch, _roll);
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _pitch * radians2Degrees,
                        min: -180.0,
                        max: 180.0,
                        divisions: 1440,
                        label: ((_pitch * radians2Degrees).toString()),
                      );
                    }),
              ),
            ),
          ]),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'Roll:  ',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _roll = value * degrees2Radians;
                              _diffQuat.setEuler(_yaw, _pitch, _roll);
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _roll * radians2Degrees,
                        min: -180.0,
                        max: 180.0,
                        divisions: 1440,
                        label: ((_roll * radians2Degrees).toString()),
                      );
                    }),
              ),
            ),
          ]),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'X: ',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _diffPos.x = value;
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _diffPos.x,
                        min: -2.0,
                        max: 2.0,
                        divisions: 400,
                        label: (_diffPos.x.toString()),
                      );
                    }),
              ),
            ),
          ]),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'Y: ',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _diffPos.y = value;
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _diffPos.y,
                        min: -2.0,
                        max: 2.0,
                        divisions: 400,
                        label: (_diffPos.y.toString()),
                      );
                    }),
              ),
            ),
          ]),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: [
            const Padding(
              padding: EdgeInsets.all(16.0),
              child: Text(
                'Z: ',
                style: TextStyle(fontSize: 16.0),
              ),
            ),
            Expanded(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: FormField<double>(
                    initialValue: 0.0,
                    builder: (FormFieldState<double> state) {
                      return Slider(
                        onChanged: (double? value) {
                          if (value != null) {
                            setState(() {
                              _diffPos.z = value;
                            });

                            calibrationClient.updateCalibration(
                                UpdateCalibrationRequest(
                                    qWorldFromDriverRotation: Quat(
                                        w: _diffQuat.w,
                                        x: _diffQuat.x,
                                        y: _diffQuat.y,
                                        z: _diffQuat.z),
                                    vecWorldFromDriverTranslation: Vec3(
                                        x: _diffPos.x,
                                        y: _diffPos.y,
                                        z: _diffPos.z)));
                          }
                          state.didChange(value);
                        },
                        value: _diffPos.z,
                        min: -2.0,
                        max: 2.0,
                        divisions: 400,
                        label: (_diffPos.z.toString()),
                      );
                    }),
              ),
            ),
          ]),
        ],
      ),
      bottomNavigationBar: const BottomAppBarControls(),
      floatingActionButton: FloatingActionButton(
        child: const Icon(Icons.compass_calibration),
        onPressed: () {
          final calMat = solveCalibrationMatrix();

          setState(() {
            _calibrationMatrix = calMat;
          });

          final calibrationTransform = Vec3(
              x: _calibrationMatrix.m[0][3],
              y: _calibrationMatrix.m[1][3],
              z: _calibrationMatrix.m[2][3]);

          final calibrationRotation = Quaternion.fromRotation(Matrix3(
            _calibrationMatrix.m[0][0],
            _calibrationMatrix.m[0][1],
            _calibrationMatrix.m[0][2],
            _calibrationMatrix.m[1][0],
            _calibrationMatrix.m[1][1],
            _calibrationMatrix.m[1][2],
            _calibrationMatrix.m[2][0],
            _calibrationMatrix.m[2][1],
            _calibrationMatrix.m[2][2],
          )).normalized();

          final calQuat = Quat(
              w: calibrationRotation.w,
              x: calibrationRotation.x,
              y: calibrationRotation.y,
              z: calibrationRotation.z);
          calibrationClient.updateCalibration(UpdateCalibrationRequest(
              qWorldFromDriverRotation: calQuat,
              vecWorldFromDriverTranslation: calibrationTransform));
        },
      ),
    );
  }
}
