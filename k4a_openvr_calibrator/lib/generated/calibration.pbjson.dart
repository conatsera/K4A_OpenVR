///
//  Generated code. Do not modify.
//  source: calibration.proto
//
// @dart = 2.12
// ignore_for_file: annotate_overrides,camel_case_types,constant_identifier_names,deprecated_member_use_from_same_package,directives_ordering,library_prefixes,non_constant_identifier_names,prefer_final_fields,return_of_invalid_type,unnecessary_const,unnecessary_import,unnecessary_this,unused_import,unused_shown_name

import 'dart:core' as $core;
import 'dart:convert' as $convert;
import 'dart:typed_data' as $typed_data;
@$core.Deprecated('Use settingsUpdateRequestDescriptor instead')
const SettingsUpdateRequest$json = const {
  '1': 'SettingsUpdateRequest',
  '2': const [
    const {'1': 'mode', '3': 1, '4': 1, '5': 14, '6': '.calibration.SettingsUpdateRequest.ProcessingMode', '10': 'mode'},
    const {'1': 'depth_res', '3': 2, '4': 1, '5': 14, '6': '.calibration.SettingsUpdateRequest.DepthResolution', '10': 'depthRes'},
    const {'1': 'smoothing', '3': 3, '4': 1, '5': 2, '10': 'smoothing'},
    const {'1': 'lite', '3': 4, '4': 1, '5': 8, '10': 'lite'},
  ],
  '4': const [SettingsUpdateRequest_ProcessingMode$json, SettingsUpdateRequest_DepthResolution$json],
};

@$core.Deprecated('Use settingsUpdateRequestDescriptor instead')
const SettingsUpdateRequest_ProcessingMode$json = const {
  '1': 'ProcessingMode',
  '2': const [
    const {'1': 'DEFAULT', '2': 0},
    const {'1': 'CPU', '2': 1},
    const {'1': 'CUDA', '2': 2},
    const {'1': 'TENSORRT', '2': 3},
    const {'1': 'DIRECTML', '2': 4},
  ],
};

@$core.Deprecated('Use settingsUpdateRequestDescriptor instead')
const SettingsUpdateRequest_DepthResolution$json = const {
  '1': 'DepthResolution',
  '2': const [
    const {'1': 'NFOV_2X2BINNED', '2': 0},
    const {'1': 'NFOV_UNBINNED', '2': 1},
    const {'1': 'WFOV_2X2BINNED', '2': 2},
    const {'1': 'WFOV_UNBINNED', '2': 3},
  ],
};

/// Descriptor for `SettingsUpdateRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List settingsUpdateRequestDescriptor = $convert.base64Decode('ChVTZXR0aW5nc1VwZGF0ZVJlcXVlc3QSRQoEbW9kZRgBIAEoDjIxLmNhbGlicmF0aW9uLlNldHRpbmdzVXBkYXRlUmVxdWVzdC5Qcm9jZXNzaW5nTW9kZVIEbW9kZRJPCglkZXB0aF9yZXMYAiABKA4yMi5jYWxpYnJhdGlvbi5TZXR0aW5nc1VwZGF0ZVJlcXVlc3QuRGVwdGhSZXNvbHV0aW9uUghkZXB0aFJlcxIcCglzbW9vdGhpbmcYAyABKAJSCXNtb290aGluZxISCgRsaXRlGAQgASgIUgRsaXRlIkwKDlByb2Nlc3NpbmdNb2RlEgsKB0RFRkFVTFQQABIHCgNDUFUQARIICgRDVURBEAISDAoIVEVOU09SUlQQAxIMCghESVJFQ1RNTBAEIl8KD0RlcHRoUmVzb2x1dGlvbhISCg5ORk9WXzJYMkJJTk5FRBAAEhEKDU5GT1ZfVU5CSU5ORUQQARISCg5XRk9WXzJYMkJJTk5FRBACEhEKDVdGT1ZfVU5CSU5ORUQQAw==');
@$core.Deprecated('Use settingsGetRequestDescriptor instead')
const SettingsGetRequest$json = const {
  '1': 'SettingsGetRequest',
};

/// Descriptor for `SettingsGetRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List settingsGetRequestDescriptor = $convert.base64Decode('ChJTZXR0aW5nc0dldFJlcXVlc3Q=');
@$core.Deprecated('Use settingsReplyDescriptor instead')
const SettingsReply$json = const {
  '1': 'SettingsReply',
  '2': const [
    const {'1': 'success', '3': 1, '4': 1, '5': 8, '10': 'success'},
  ],
};

/// Descriptor for `SettingsReply`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List settingsReplyDescriptor = $convert.base64Decode('Cg1TZXR0aW5nc1JlcGx5EhgKB3N1Y2Nlc3MYASABKAhSB3N1Y2Nlc3M=');
@$core.Deprecated('Use pauseResumeRequestDescriptor instead')
const PauseResumeRequest$json = const {
  '1': 'PauseResumeRequest',
};

/// Descriptor for `PauseResumeRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List pauseResumeRequestDescriptor = $convert.base64Decode('ChJQYXVzZVJlc3VtZVJlcXVlc3Q=');
@$core.Deprecated('Use pauseResumeReplyDescriptor instead')
const PauseResumeReply$json = const {
  '1': 'PauseResumeReply',
  '2': const [
    const {'1': 'paused', '3': 1, '4': 1, '5': 8, '10': 'paused'},
  ],
};

/// Descriptor for `PauseResumeReply`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List pauseResumeReplyDescriptor = $convert.base64Decode('ChBQYXVzZVJlc3VtZVJlcGx5EhYKBnBhdXNlZBgBIAEoCFIGcGF1c2Vk');
@$core.Deprecated('Use vec3Descriptor instead')
const Vec3$json = const {
  '1': 'Vec3',
  '2': const [
    const {'1': 'x', '3': 1, '4': 1, '5': 1, '10': 'x'},
    const {'1': 'y', '3': 2, '4': 1, '5': 1, '10': 'y'},
    const {'1': 'z', '3': 3, '4': 1, '5': 1, '10': 'z'},
  ],
};

/// Descriptor for `Vec3`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List vec3Descriptor = $convert.base64Decode('CgRWZWMzEgwKAXgYASABKAFSAXgSDAoBeRgCIAEoAVIBeRIMCgF6GAMgASgBUgF6');
@$core.Deprecated('Use quatDescriptor instead')
const Quat$json = const {
  '1': 'Quat',
  '2': const [
    const {'1': 'w', '3': 1, '4': 1, '5': 1, '10': 'w'},
    const {'1': 'x', '3': 2, '4': 1, '5': 1, '10': 'x'},
    const {'1': 'y', '3': 3, '4': 1, '5': 1, '10': 'y'},
    const {'1': 'z', '3': 4, '4': 1, '5': 1, '10': 'z'},
  ],
};

/// Descriptor for `Quat`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List quatDescriptor = $convert.base64Decode('CgRRdWF0EgwKAXcYASABKAFSAXcSDAoBeBgCIAEoAVIBeBIMCgF5GAMgASgBUgF5EgwKAXoYBCABKAFSAXo=');
@$core.Deprecated('Use poseDescriptor instead')
const Pose$json = const {
  '1': 'Pose',
  '2': const [
    const {'1': 'pos', '3': 1, '4': 1, '5': 11, '6': '.calibration.Vec3', '10': 'pos'},
    const {'1': 'rot', '3': 2, '4': 1, '5': 11, '6': '.calibration.Quat', '10': 'rot'},
  ],
};

/// Descriptor for `Pose`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List poseDescriptor = $convert.base64Decode('CgRQb3NlEiMKA3BvcxgBIAEoCzIRLmNhbGlicmF0aW9uLlZlYzNSA3BvcxIjCgNyb3QYAiABKAsyES5jYWxpYnJhdGlvbi5RdWF0UgNyb3Q=');
@$core.Deprecated('Use driverMatrix4Descriptor instead')
const DriverMatrix4$json = const {
  '1': 'DriverMatrix4',
  '2': const [
    const {'1': 'm00', '3': 1, '4': 1, '5': 2, '10': 'm00'},
    const {'1': 'm01', '3': 2, '4': 1, '5': 2, '10': 'm01'},
    const {'1': 'm02', '3': 3, '4': 1, '5': 2, '10': 'm02'},
    const {'1': 'm03', '3': 4, '4': 1, '5': 2, '10': 'm03'},
    const {'1': 'm10', '3': 5, '4': 1, '5': 2, '10': 'm10'},
    const {'1': 'm11', '3': 6, '4': 1, '5': 2, '10': 'm11'},
    const {'1': 'm12', '3': 7, '4': 1, '5': 2, '10': 'm12'},
    const {'1': 'm13', '3': 8, '4': 1, '5': 2, '10': 'm13'},
    const {'1': 'm20', '3': 9, '4': 1, '5': 2, '10': 'm20'},
    const {'1': 'm21', '3': 10, '4': 1, '5': 2, '10': 'm21'},
    const {'1': 'm22', '3': 11, '4': 1, '5': 2, '10': 'm22'},
    const {'1': 'm23', '3': 12, '4': 1, '5': 2, '10': 'm23'},
  ],
};

/// Descriptor for `DriverMatrix4`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List driverMatrix4Descriptor = $convert.base64Decode('Cg1Ecml2ZXJNYXRyaXg0EhAKA20wMBgBIAEoAlIDbTAwEhAKA20wMRgCIAEoAlIDbTAxEhAKA20wMhgDIAEoAlIDbTAyEhAKA20wMxgEIAEoAlIDbTAzEhAKA20xMBgFIAEoAlIDbTEwEhAKA20xMRgGIAEoAlIDbTExEhAKA20xMhgHIAEoAlIDbTEyEhAKA20xMxgIIAEoAlIDbTEzEhAKA20yMBgJIAEoAlIDbTIwEhAKA20yMRgKIAEoAlIDbTIxEhAKA20yMhgLIAEoAlIDbTIyEhAKA20yMxgMIAEoAlIDbTIz');
@$core.Deprecated('Use driverPoseDescriptor instead')
const DriverPose$json = const {
  '1': 'DriverPose',
  '2': const [
    const {'1': 'pose', '3': 1, '4': 1, '5': 11, '6': '.calibration.DriverMatrix4', '10': 'pose'},
    const {'1': 'ts', '3': 2, '4': 1, '5': 4, '10': 'ts'},
  ],
};

/// Descriptor for `DriverPose`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List driverPoseDescriptor = $convert.base64Decode('CgpEcml2ZXJQb3NlEi4KBHBvc2UYASABKAsyGi5jYWxpYnJhdGlvbi5Ecml2ZXJNYXRyaXg0UgRwb3NlEg4KAnRzGAIgASgEUgJ0cw==');
@$core.Deprecated('Use k4ABTPoseDescriptor instead')
const K4ABTPose$json = const {
  '1': 'K4ABTPose',
  '2': const [
    const {'1': 'pose', '3': 1, '4': 1, '5': 11, '6': '.calibration.Pose', '10': 'pose'},
    const {'1': 'dev_ts', '3': 2, '4': 1, '5': 4, '10': 'devTs'},
    const {'1': 'sys_ts', '3': 3, '4': 1, '5': 4, '10': 'sysTs'},
  ],
};

/// Descriptor for `K4ABTPose`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List k4ABTPoseDescriptor = $convert.base64Decode('CglLNEFCVFBvc2USJQoEcG9zZRgBIAEoCzIRLmNhbGlicmF0aW9uLlBvc2VSBHBvc2USFQoGZGV2X3RzGAIgASgEUgVkZXZUcxIVCgZzeXNfdHMYAyABKARSBXN5c1Rz');
@$core.Deprecated('Use imuSampleDescriptor instead')
const ImuSample$json = const {
  '1': 'ImuSample',
  '2': const [
    const {'1': 'sample', '3': 1, '4': 1, '5': 11, '6': '.calibration.Vec3', '10': 'sample'},
    const {'1': 'ts', '3': 2, '4': 1, '5': 4, '10': 'ts'},
  ],
};

/// Descriptor for `ImuSample`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List imuSampleDescriptor = $convert.base64Decode('CglJbXVTYW1wbGUSKQoGc2FtcGxlGAEgASgLMhEuY2FsaWJyYXRpb24uVmVjM1IGc2FtcGxlEg4KAnRzGAIgASgEUgJ0cw==');
@$core.Deprecated('Use calibrationDataDescriptor instead')
const CalibrationData$json = const {
  '1': 'CalibrationData',
  '2': const [
    const {'1': 'hmd', '3': 1, '4': 1, '5': 11, '6': '.calibration.DriverPose', '10': 'hmd'},
    const {'1': 'head', '3': 2, '4': 1, '5': 11, '6': '.calibration.K4ABTPose', '10': 'head'},
  ],
};

/// Descriptor for `CalibrationData`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List calibrationDataDescriptor = $convert.base64Decode('Cg9DYWxpYnJhdGlvbkRhdGESKQoDaG1kGAEgASgLMhcuY2FsaWJyYXRpb24uRHJpdmVyUG9zZVIDaG1kEioKBGhlYWQYAiABKAsyFi5jYWxpYnJhdGlvbi5LNEFCVFBvc2VSBGhlYWQ=');
@$core.Deprecated('Use calibrationDataRequestDescriptor instead')
const CalibrationDataRequest$json = const {
  '1': 'CalibrationDataRequest',
};

/// Descriptor for `CalibrationDataRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List calibrationDataRequestDescriptor = $convert.base64Decode('ChZDYWxpYnJhdGlvbkRhdGFSZXF1ZXN0');
@$core.Deprecated('Use calibrationDataReplyDescriptor instead')
const CalibrationDataReply$json = const {
  '1': 'CalibrationDataReply',
  '2': const [
    const {'1': 'success', '3': 1, '4': 1, '5': 8, '10': 'success'},
    const {'1': 'acc', '3': 2, '4': 1, '5': 11, '6': '.calibration.ImuSample', '10': 'acc'},
    const {'1': 'gyro', '3': 3, '4': 1, '5': 11, '6': '.calibration.ImuSample', '10': 'gyro'},
    const {'1': 'data', '3': 4, '4': 1, '5': 11, '6': '.calibration.CalibrationData', '10': 'data'},
  ],
};

/// Descriptor for `CalibrationDataReply`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List calibrationDataReplyDescriptor = $convert.base64Decode('ChRDYWxpYnJhdGlvbkRhdGFSZXBseRIYCgdzdWNjZXNzGAEgASgIUgdzdWNjZXNzEigKA2FjYxgCIAEoCzIWLmNhbGlicmF0aW9uLkltdVNhbXBsZVIDYWNjEioKBGd5cm8YAyABKAsyFi5jYWxpYnJhdGlvbi5JbXVTYW1wbGVSBGd5cm8SMAoEZGF0YRgEIAEoCzIcLmNhbGlicmF0aW9uLkNhbGlicmF0aW9uRGF0YVIEZGF0YQ==');
@$core.Deprecated('Use updateCalibrationRequestDescriptor instead')
const UpdateCalibrationRequest$json = const {
  '1': 'UpdateCalibrationRequest',
  '2': const [
    const {'1': 'q_world_from_driver_rotation', '3': 1, '4': 1, '5': 11, '6': '.calibration.Quat', '10': 'qWorldFromDriverRotation'},
    const {'1': 'vec_world_from_driver_translation', '3': 2, '4': 1, '5': 11, '6': '.calibration.Vec3', '10': 'vecWorldFromDriverTranslation'},
  ],
};

/// Descriptor for `UpdateCalibrationRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List updateCalibrationRequestDescriptor = $convert.base64Decode('ChhVcGRhdGVDYWxpYnJhdGlvblJlcXVlc3QSUQoccV93b3JsZF9mcm9tX2RyaXZlcl9yb3RhdGlvbhgBIAEoCzIRLmNhbGlicmF0aW9uLlF1YXRSGHFXb3JsZEZyb21Ecml2ZXJSb3RhdGlvbhJbCiF2ZWNfd29ybGRfZnJvbV9kcml2ZXJfdHJhbnNsYXRpb24YAiABKAsyES5jYWxpYnJhdGlvbi5WZWMzUh12ZWNXb3JsZEZyb21Ecml2ZXJUcmFuc2xhdGlvbg==');
@$core.Deprecated('Use updateCalibrationReplyDescriptor instead')
const UpdateCalibrationReply$json = const {
  '1': 'UpdateCalibrationReply',
  '2': const [
    const {'1': 'success', '3': 1, '4': 1, '5': 8, '10': 'success'},
  ],
};

/// Descriptor for `UpdateCalibrationReply`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List updateCalibrationReplyDescriptor = $convert.base64Decode('ChZVcGRhdGVDYWxpYnJhdGlvblJlcGx5EhgKB3N1Y2Nlc3MYASABKAhSB3N1Y2Nlc3M=');
