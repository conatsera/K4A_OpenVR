///
//  Generated code. Do not modify.
//  source: calibration.proto
//
// @dart = 2.12
// ignore_for_file: annotate_overrides,camel_case_types,constant_identifier_names,directives_ordering,library_prefixes,non_constant_identifier_names,prefer_final_fields,return_of_invalid_type,unnecessary_const,unnecessary_import,unnecessary_this,unused_import,unused_shown_name

import 'dart:async' as $async;

import 'dart:core' as $core;

import 'package:grpc/service_api.dart' as $grpc;
import 'calibration.pb.dart' as $0;
export 'calibration.pb.dart';

class SettingsClient extends $grpc.Client {
  static final _$get =
      $grpc.ClientMethod<$0.SettingsGetRequest, $0.SettingsUpdateRequest>(
          '/calibration.Settings/Get',
          ($0.SettingsGetRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $0.SettingsUpdateRequest.fromBuffer(value));
  static final _$update =
      $grpc.ClientMethod<$0.SettingsUpdateRequest, $0.SettingsReply>(
          '/calibration.Settings/Update',
          ($0.SettingsUpdateRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) => $0.SettingsReply.fromBuffer(value));
  static final _$pauseResume =
      $grpc.ClientMethod<$0.PauseResumeRequest, $0.PauseResumeReply>(
          '/calibration.Settings/PauseResume',
          ($0.PauseResumeRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $0.PauseResumeReply.fromBuffer(value));

  SettingsClient($grpc.ClientChannel channel,
      {$grpc.CallOptions? options,
      $core.Iterable<$grpc.ClientInterceptor>? interceptors})
      : super(channel, options: options, interceptors: interceptors);

  $grpc.ResponseFuture<$0.SettingsUpdateRequest> get(
      $0.SettingsGetRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$get, request, options: options);
  }

  $grpc.ResponseFuture<$0.SettingsReply> update(
      $0.SettingsUpdateRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$update, request, options: options);
  }

  $grpc.ResponseFuture<$0.PauseResumeReply> pauseResume(
      $0.PauseResumeRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$pauseResume, request, options: options);
  }
}

abstract class SettingsServiceBase extends $grpc.Service {
  $core.String get $name => 'calibration.Settings';

  SettingsServiceBase() {
    $addMethod(
        $grpc.ServiceMethod<$0.SettingsGetRequest, $0.SettingsUpdateRequest>(
            'Get',
            get_Pre,
            false,
            false,
            ($core.List<$core.int> value) =>
                $0.SettingsGetRequest.fromBuffer(value),
            ($0.SettingsUpdateRequest value) => value.writeToBuffer()));
    $addMethod($grpc.ServiceMethod<$0.SettingsUpdateRequest, $0.SettingsReply>(
        'Update',
        update_Pre,
        false,
        false,
        ($core.List<$core.int> value) =>
            $0.SettingsUpdateRequest.fromBuffer(value),
        ($0.SettingsReply value) => value.writeToBuffer()));
    $addMethod($grpc.ServiceMethod<$0.PauseResumeRequest, $0.PauseResumeReply>(
        'PauseResume',
        pauseResume_Pre,
        false,
        false,
        ($core.List<$core.int> value) =>
            $0.PauseResumeRequest.fromBuffer(value),
        ($0.PauseResumeReply value) => value.writeToBuffer()));
  }

  $async.Future<$0.SettingsUpdateRequest> get_Pre($grpc.ServiceCall call,
      $async.Future<$0.SettingsGetRequest> request) async {
    return get(call, await request);
  }

  $async.Future<$0.SettingsReply> update_Pre($grpc.ServiceCall call,
      $async.Future<$0.SettingsUpdateRequest> request) async {
    return update(call, await request);
  }

  $async.Future<$0.PauseResumeReply> pauseResume_Pre($grpc.ServiceCall call,
      $async.Future<$0.PauseResumeRequest> request) async {
    return pauseResume(call, await request);
  }

  $async.Future<$0.SettingsUpdateRequest> get(
      $grpc.ServiceCall call, $0.SettingsGetRequest request);
  $async.Future<$0.SettingsReply> update(
      $grpc.ServiceCall call, $0.SettingsUpdateRequest request);
  $async.Future<$0.PauseResumeReply> pauseResume(
      $grpc.ServiceCall call, $0.PauseResumeRequest request);
}

class CalibrationClient extends $grpc.Client {
  static final _$getCalibrationData =
      $grpc.ClientMethod<$0.CalibrationDataRequest, $0.CalibrationDataReply>(
          '/calibration.Calibration/GetCalibrationData',
          ($0.CalibrationDataRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $0.CalibrationDataReply.fromBuffer(value));
  static final _$updateCalibration = $grpc.ClientMethod<
          $0.UpdateCalibrationRequest, $0.UpdateCalibrationReply>(
      '/calibration.Calibration/UpdateCalibration',
      ($0.UpdateCalibrationRequest value) => value.writeToBuffer(),
      ($core.List<$core.int> value) =>
          $0.UpdateCalibrationReply.fromBuffer(value));
  static final _$getPoseStream =
      $grpc.ClientMethod<$0.CalibrationDataRequest, $0.CalibrationData>(
          '/calibration.Calibration/GetPoseStream',
          ($0.CalibrationDataRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $0.CalibrationData.fromBuffer(value));
  static final _$stopPoseStream =
      $grpc.ClientMethod<$0.CalibrationDataRequest, $0.UpdateCalibrationReply>(
          '/calibration.Calibration/StopPoseStream',
          ($0.CalibrationDataRequest value) => value.writeToBuffer(),
          ($core.List<$core.int> value) =>
              $0.UpdateCalibrationReply.fromBuffer(value));

  CalibrationClient($grpc.ClientChannel channel,
      {$grpc.CallOptions? options,
      $core.Iterable<$grpc.ClientInterceptor>? interceptors})
      : super(channel, options: options, interceptors: interceptors);

  $grpc.ResponseFuture<$0.CalibrationDataReply> getCalibrationData(
      $0.CalibrationDataRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$getCalibrationData, request, options: options);
  }

  $grpc.ResponseFuture<$0.UpdateCalibrationReply> updateCalibration(
      $0.UpdateCalibrationRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$updateCalibration, request, options: options);
  }

  $grpc.ResponseStream<$0.CalibrationData> getPoseStream(
      $0.CalibrationDataRequest request,
      {$grpc.CallOptions? options}) {
    return $createStreamingCall(
        _$getPoseStream, $async.Stream.fromIterable([request]),
        options: options);
  }

  $grpc.ResponseFuture<$0.UpdateCalibrationReply> stopPoseStream(
      $0.CalibrationDataRequest request,
      {$grpc.CallOptions? options}) {
    return $createUnaryCall(_$stopPoseStream, request, options: options);
  }
}

abstract class CalibrationServiceBase extends $grpc.Service {
  $core.String get $name => 'calibration.Calibration';

  CalibrationServiceBase() {
    $addMethod(
        $grpc.ServiceMethod<$0.CalibrationDataRequest, $0.CalibrationDataReply>(
            'GetCalibrationData',
            getCalibrationData_Pre,
            false,
            false,
            ($core.List<$core.int> value) =>
                $0.CalibrationDataRequest.fromBuffer(value),
            ($0.CalibrationDataReply value) => value.writeToBuffer()));
    $addMethod($grpc.ServiceMethod<$0.UpdateCalibrationRequest,
            $0.UpdateCalibrationReply>(
        'UpdateCalibration',
        updateCalibration_Pre,
        false,
        false,
        ($core.List<$core.int> value) =>
            $0.UpdateCalibrationRequest.fromBuffer(value),
        ($0.UpdateCalibrationReply value) => value.writeToBuffer()));
    $addMethod(
        $grpc.ServiceMethod<$0.CalibrationDataRequest, $0.CalibrationData>(
            'GetPoseStream',
            getPoseStream_Pre,
            false,
            true,
            ($core.List<$core.int> value) =>
                $0.CalibrationDataRequest.fromBuffer(value),
            ($0.CalibrationData value) => value.writeToBuffer()));
    $addMethod($grpc.ServiceMethod<$0.CalibrationDataRequest,
            $0.UpdateCalibrationReply>(
        'StopPoseStream',
        stopPoseStream_Pre,
        false,
        false,
        ($core.List<$core.int> value) =>
            $0.CalibrationDataRequest.fromBuffer(value),
        ($0.UpdateCalibrationReply value) => value.writeToBuffer()));
  }

  $async.Future<$0.CalibrationDataReply> getCalibrationData_Pre(
      $grpc.ServiceCall call,
      $async.Future<$0.CalibrationDataRequest> request) async {
    return getCalibrationData(call, await request);
  }

  $async.Future<$0.UpdateCalibrationReply> updateCalibration_Pre(
      $grpc.ServiceCall call,
      $async.Future<$0.UpdateCalibrationRequest> request) async {
    return updateCalibration(call, await request);
  }

  $async.Stream<$0.CalibrationData> getPoseStream_Pre($grpc.ServiceCall call,
      $async.Future<$0.CalibrationDataRequest> request) async* {
    yield* getPoseStream(call, await request);
  }

  $async.Future<$0.UpdateCalibrationReply> stopPoseStream_Pre(
      $grpc.ServiceCall call,
      $async.Future<$0.CalibrationDataRequest> request) async {
    return stopPoseStream(call, await request);
  }

  $async.Future<$0.CalibrationDataReply> getCalibrationData(
      $grpc.ServiceCall call, $0.CalibrationDataRequest request);
  $async.Future<$0.UpdateCalibrationReply> updateCalibration(
      $grpc.ServiceCall call, $0.UpdateCalibrationRequest request);
  $async.Stream<$0.CalibrationData> getPoseStream(
      $grpc.ServiceCall call, $0.CalibrationDataRequest request);
  $async.Future<$0.UpdateCalibrationReply> stopPoseStream(
      $grpc.ServiceCall call, $0.CalibrationDataRequest request);
}
