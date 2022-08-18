///
//  Generated code. Do not modify.
//  source: calibration.proto
//
// @dart = 2.12
// ignore_for_file: annotate_overrides,camel_case_types,constant_identifier_names,directives_ordering,library_prefixes,non_constant_identifier_names,prefer_final_fields,return_of_invalid_type,unnecessary_const,unnecessary_import,unnecessary_this,unused_import,unused_shown_name

// ignore_for_file: UNDEFINED_SHOWN_NAME
import 'dart:core' as $core;
import 'package:protobuf/protobuf.dart' as $pb;

class SettingsUpdateRequest_ProcessingMode extends $pb.ProtobufEnum {
  static const SettingsUpdateRequest_ProcessingMode DEFAULT = SettingsUpdateRequest_ProcessingMode._(0, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'DEFAULT');
  static const SettingsUpdateRequest_ProcessingMode CPU = SettingsUpdateRequest_ProcessingMode._(1, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'CPU');
  static const SettingsUpdateRequest_ProcessingMode CUDA = SettingsUpdateRequest_ProcessingMode._(2, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'CUDA');
  static const SettingsUpdateRequest_ProcessingMode TENSORRT = SettingsUpdateRequest_ProcessingMode._(3, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'TENSORRT');
  static const SettingsUpdateRequest_ProcessingMode DIRECTML = SettingsUpdateRequest_ProcessingMode._(4, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'DIRECTML');

  static const $core.List<SettingsUpdateRequest_ProcessingMode> values = <SettingsUpdateRequest_ProcessingMode> [
    DEFAULT,
    CPU,
    CUDA,
    TENSORRT,
    DIRECTML,
  ];

  static final $core.Map<$core.int, SettingsUpdateRequest_ProcessingMode> _byValue = $pb.ProtobufEnum.initByValue(values);
  static SettingsUpdateRequest_ProcessingMode? valueOf($core.int value) => _byValue[value];

  const SettingsUpdateRequest_ProcessingMode._($core.int v, $core.String n) : super(v, n);
}

class SettingsUpdateRequest_DepthResolution extends $pb.ProtobufEnum {
  static const SettingsUpdateRequest_DepthResolution NFOV_2X2BINNED = SettingsUpdateRequest_DepthResolution._(0, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'NFOV_2X2BINNED');
  static const SettingsUpdateRequest_DepthResolution NFOV_UNBINNED = SettingsUpdateRequest_DepthResolution._(1, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'NFOV_UNBINNED');
  static const SettingsUpdateRequest_DepthResolution WFOV_2X2BINNED = SettingsUpdateRequest_DepthResolution._(2, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'WFOV_2X2BINNED');
  static const SettingsUpdateRequest_DepthResolution WFOV_UNBINNED = SettingsUpdateRequest_DepthResolution._(3, const $core.bool.fromEnvironment('protobuf.omit_enum_names') ? '' : 'WFOV_UNBINNED');

  static const $core.List<SettingsUpdateRequest_DepthResolution> values = <SettingsUpdateRequest_DepthResolution> [
    NFOV_2X2BINNED,
    NFOV_UNBINNED,
    WFOV_2X2BINNED,
    WFOV_UNBINNED,
  ];

  static final $core.Map<$core.int, SettingsUpdateRequest_DepthResolution> _byValue = $pb.ProtobufEnum.initByValue(values);
  static SettingsUpdateRequest_DepthResolution? valueOf($core.int value) => _byValue[value];

  const SettingsUpdateRequest_DepthResolution._($core.int v, $core.String n) : super(v, n);
}

