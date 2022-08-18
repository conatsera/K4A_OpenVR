///
//  Generated code. Do not modify.
//  source: calibration.proto
//
// @dart = 2.12
// ignore_for_file: annotate_overrides,camel_case_types,constant_identifier_names,directives_ordering,library_prefixes,non_constant_identifier_names,prefer_final_fields,return_of_invalid_type,unnecessary_const,unnecessary_import,unnecessary_this,unused_import,unused_shown_name

import 'dart:core' as $core;

import 'package:fixnum/fixnum.dart' as $fixnum;
import 'package:protobuf/protobuf.dart' as $pb;

import 'calibration.pbenum.dart';

export 'calibration.pbenum.dart';

class SettingsUpdateRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'SettingsUpdateRequest', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..e<SettingsUpdateRequest_ProcessingMode>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'mode', $pb.PbFieldType.OE, defaultOrMaker: SettingsUpdateRequest_ProcessingMode.DEFAULT, valueOf: SettingsUpdateRequest_ProcessingMode.valueOf, enumValues: SettingsUpdateRequest_ProcessingMode.values)
    ..e<SettingsUpdateRequest_DepthResolution>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'depthRes', $pb.PbFieldType.OE, defaultOrMaker: SettingsUpdateRequest_DepthResolution.NFOV_2X2BINNED, valueOf: SettingsUpdateRequest_DepthResolution.valueOf, enumValues: SettingsUpdateRequest_DepthResolution.values)
    ..a<$core.double>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'smoothing', $pb.PbFieldType.OF)
    ..aOB(4, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'lite')
    ..hasRequiredFields = false
  ;

  SettingsUpdateRequest._() : super();
  factory SettingsUpdateRequest({
    SettingsUpdateRequest_ProcessingMode? mode,
    SettingsUpdateRequest_DepthResolution? depthRes,
    $core.double? smoothing,
    $core.bool? lite,
  }) {
    final _result = create();
    if (mode != null) {
      _result.mode = mode;
    }
    if (depthRes != null) {
      _result.depthRes = depthRes;
    }
    if (smoothing != null) {
      _result.smoothing = smoothing;
    }
    if (lite != null) {
      _result.lite = lite;
    }
    return _result;
  }
  factory SettingsUpdateRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory SettingsUpdateRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  SettingsUpdateRequest clone() => SettingsUpdateRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  SettingsUpdateRequest copyWith(void Function(SettingsUpdateRequest) updates) => super.copyWith((message) => updates(message as SettingsUpdateRequest)) as SettingsUpdateRequest; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static SettingsUpdateRequest create() => SettingsUpdateRequest._();
  SettingsUpdateRequest createEmptyInstance() => create();
  static $pb.PbList<SettingsUpdateRequest> createRepeated() => $pb.PbList<SettingsUpdateRequest>();
  @$core.pragma('dart2js:noInline')
  static SettingsUpdateRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<SettingsUpdateRequest>(create);
  static SettingsUpdateRequest? _defaultInstance;

  @$pb.TagNumber(1)
  SettingsUpdateRequest_ProcessingMode get mode => $_getN(0);
  @$pb.TagNumber(1)
  set mode(SettingsUpdateRequest_ProcessingMode v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasMode() => $_has(0);
  @$pb.TagNumber(1)
  void clearMode() => clearField(1);

  @$pb.TagNumber(2)
  SettingsUpdateRequest_DepthResolution get depthRes => $_getN(1);
  @$pb.TagNumber(2)
  set depthRes(SettingsUpdateRequest_DepthResolution v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasDepthRes() => $_has(1);
  @$pb.TagNumber(2)
  void clearDepthRes() => clearField(2);

  @$pb.TagNumber(3)
  $core.double get smoothing => $_getN(2);
  @$pb.TagNumber(3)
  set smoothing($core.double v) { $_setFloat(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasSmoothing() => $_has(2);
  @$pb.TagNumber(3)
  void clearSmoothing() => clearField(3);

  @$pb.TagNumber(4)
  $core.bool get lite => $_getBF(3);
  @$pb.TagNumber(4)
  set lite($core.bool v) { $_setBool(3, v); }
  @$pb.TagNumber(4)
  $core.bool hasLite() => $_has(3);
  @$pb.TagNumber(4)
  void clearLite() => clearField(4);
}

class SettingsGetRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'SettingsGetRequest', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  SettingsGetRequest._() : super();
  factory SettingsGetRequest() => create();
  factory SettingsGetRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory SettingsGetRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  SettingsGetRequest clone() => SettingsGetRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  SettingsGetRequest copyWith(void Function(SettingsGetRequest) updates) => super.copyWith((message) => updates(message as SettingsGetRequest)) as SettingsGetRequest; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static SettingsGetRequest create() => SettingsGetRequest._();
  SettingsGetRequest createEmptyInstance() => create();
  static $pb.PbList<SettingsGetRequest> createRepeated() => $pb.PbList<SettingsGetRequest>();
  @$core.pragma('dart2js:noInline')
  static SettingsGetRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<SettingsGetRequest>(create);
  static SettingsGetRequest? _defaultInstance;
}

class SettingsReply extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'SettingsReply', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOB(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'success')
    ..hasRequiredFields = false
  ;

  SettingsReply._() : super();
  factory SettingsReply({
    $core.bool? success,
  }) {
    final _result = create();
    if (success != null) {
      _result.success = success;
    }
    return _result;
  }
  factory SettingsReply.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory SettingsReply.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  SettingsReply clone() => SettingsReply()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  SettingsReply copyWith(void Function(SettingsReply) updates) => super.copyWith((message) => updates(message as SettingsReply)) as SettingsReply; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static SettingsReply create() => SettingsReply._();
  SettingsReply createEmptyInstance() => create();
  static $pb.PbList<SettingsReply> createRepeated() => $pb.PbList<SettingsReply>();
  @$core.pragma('dart2js:noInline')
  static SettingsReply getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<SettingsReply>(create);
  static SettingsReply? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get success => $_getBF(0);
  @$pb.TagNumber(1)
  set success($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasSuccess() => $_has(0);
  @$pb.TagNumber(1)
  void clearSuccess() => clearField(1);
}

class PauseResumeRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'PauseResumeRequest', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  PauseResumeRequest._() : super();
  factory PauseResumeRequest() => create();
  factory PauseResumeRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory PauseResumeRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  PauseResumeRequest clone() => PauseResumeRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  PauseResumeRequest copyWith(void Function(PauseResumeRequest) updates) => super.copyWith((message) => updates(message as PauseResumeRequest)) as PauseResumeRequest; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static PauseResumeRequest create() => PauseResumeRequest._();
  PauseResumeRequest createEmptyInstance() => create();
  static $pb.PbList<PauseResumeRequest> createRepeated() => $pb.PbList<PauseResumeRequest>();
  @$core.pragma('dart2js:noInline')
  static PauseResumeRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<PauseResumeRequest>(create);
  static PauseResumeRequest? _defaultInstance;
}

class PauseResumeReply extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'PauseResumeReply', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOB(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'paused')
    ..hasRequiredFields = false
  ;

  PauseResumeReply._() : super();
  factory PauseResumeReply({
    $core.bool? paused,
  }) {
    final _result = create();
    if (paused != null) {
      _result.paused = paused;
    }
    return _result;
  }
  factory PauseResumeReply.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory PauseResumeReply.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  PauseResumeReply clone() => PauseResumeReply()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  PauseResumeReply copyWith(void Function(PauseResumeReply) updates) => super.copyWith((message) => updates(message as PauseResumeReply)) as PauseResumeReply; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static PauseResumeReply create() => PauseResumeReply._();
  PauseResumeReply createEmptyInstance() => create();
  static $pb.PbList<PauseResumeReply> createRepeated() => $pb.PbList<PauseResumeReply>();
  @$core.pragma('dart2js:noInline')
  static PauseResumeReply getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<PauseResumeReply>(create);
  static PauseResumeReply? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get paused => $_getBF(0);
  @$pb.TagNumber(1)
  set paused($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasPaused() => $_has(0);
  @$pb.TagNumber(1)
  void clearPaused() => clearField(1);
}

class Vec3 extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'Vec3', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..a<$core.double>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'x', $pb.PbFieldType.OD)
    ..a<$core.double>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'y', $pb.PbFieldType.OD)
    ..a<$core.double>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'z', $pb.PbFieldType.OD)
    ..hasRequiredFields = false
  ;

  Vec3._() : super();
  factory Vec3({
    $core.double? x,
    $core.double? y,
    $core.double? z,
  }) {
    final _result = create();
    if (x != null) {
      _result.x = x;
    }
    if (y != null) {
      _result.y = y;
    }
    if (z != null) {
      _result.z = z;
    }
    return _result;
  }
  factory Vec3.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Vec3.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Vec3 clone() => Vec3()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Vec3 copyWith(void Function(Vec3) updates) => super.copyWith((message) => updates(message as Vec3)) as Vec3; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static Vec3 create() => Vec3._();
  Vec3 createEmptyInstance() => create();
  static $pb.PbList<Vec3> createRepeated() => $pb.PbList<Vec3>();
  @$core.pragma('dart2js:noInline')
  static Vec3 getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Vec3>(create);
  static Vec3? _defaultInstance;

  @$pb.TagNumber(1)
  $core.double get x => $_getN(0);
  @$pb.TagNumber(1)
  set x($core.double v) { $_setDouble(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasX() => $_has(0);
  @$pb.TagNumber(1)
  void clearX() => clearField(1);

  @$pb.TagNumber(2)
  $core.double get y => $_getN(1);
  @$pb.TagNumber(2)
  set y($core.double v) { $_setDouble(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasY() => $_has(1);
  @$pb.TagNumber(2)
  void clearY() => clearField(2);

  @$pb.TagNumber(3)
  $core.double get z => $_getN(2);
  @$pb.TagNumber(3)
  set z($core.double v) { $_setDouble(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasZ() => $_has(2);
  @$pb.TagNumber(3)
  void clearZ() => clearField(3);
}

class Quat extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'Quat', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..a<$core.double>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'w', $pb.PbFieldType.OD)
    ..a<$core.double>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'x', $pb.PbFieldType.OD)
    ..a<$core.double>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'y', $pb.PbFieldType.OD)
    ..a<$core.double>(4, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'z', $pb.PbFieldType.OD)
    ..hasRequiredFields = false
  ;

  Quat._() : super();
  factory Quat({
    $core.double? w,
    $core.double? x,
    $core.double? y,
    $core.double? z,
  }) {
    final _result = create();
    if (w != null) {
      _result.w = w;
    }
    if (x != null) {
      _result.x = x;
    }
    if (y != null) {
      _result.y = y;
    }
    if (z != null) {
      _result.z = z;
    }
    return _result;
  }
  factory Quat.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Quat.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Quat clone() => Quat()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Quat copyWith(void Function(Quat) updates) => super.copyWith((message) => updates(message as Quat)) as Quat; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static Quat create() => Quat._();
  Quat createEmptyInstance() => create();
  static $pb.PbList<Quat> createRepeated() => $pb.PbList<Quat>();
  @$core.pragma('dart2js:noInline')
  static Quat getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Quat>(create);
  static Quat? _defaultInstance;

  @$pb.TagNumber(1)
  $core.double get w => $_getN(0);
  @$pb.TagNumber(1)
  set w($core.double v) { $_setDouble(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasW() => $_has(0);
  @$pb.TagNumber(1)
  void clearW() => clearField(1);

  @$pb.TagNumber(2)
  $core.double get x => $_getN(1);
  @$pb.TagNumber(2)
  set x($core.double v) { $_setDouble(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasX() => $_has(1);
  @$pb.TagNumber(2)
  void clearX() => clearField(2);

  @$pb.TagNumber(3)
  $core.double get y => $_getN(2);
  @$pb.TagNumber(3)
  set y($core.double v) { $_setDouble(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasY() => $_has(2);
  @$pb.TagNumber(3)
  void clearY() => clearField(3);

  @$pb.TagNumber(4)
  $core.double get z => $_getN(3);
  @$pb.TagNumber(4)
  set z($core.double v) { $_setDouble(3, v); }
  @$pb.TagNumber(4)
  $core.bool hasZ() => $_has(3);
  @$pb.TagNumber(4)
  void clearZ() => clearField(4);
}

class Pose extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'Pose', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<Vec3>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'pos', subBuilder: Vec3.create)
    ..aOM<Quat>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'rot', subBuilder: Quat.create)
    ..hasRequiredFields = false
  ;

  Pose._() : super();
  factory Pose({
    Vec3? pos,
    Quat? rot,
  }) {
    final _result = create();
    if (pos != null) {
      _result.pos = pos;
    }
    if (rot != null) {
      _result.rot = rot;
    }
    return _result;
  }
  factory Pose.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Pose.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Pose clone() => Pose()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Pose copyWith(void Function(Pose) updates) => super.copyWith((message) => updates(message as Pose)) as Pose; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static Pose create() => Pose._();
  Pose createEmptyInstance() => create();
  static $pb.PbList<Pose> createRepeated() => $pb.PbList<Pose>();
  @$core.pragma('dart2js:noInline')
  static Pose getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Pose>(create);
  static Pose? _defaultInstance;

  @$pb.TagNumber(1)
  Vec3 get pos => $_getN(0);
  @$pb.TagNumber(1)
  set pos(Vec3 v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPos() => $_has(0);
  @$pb.TagNumber(1)
  void clearPos() => clearField(1);
  @$pb.TagNumber(1)
  Vec3 ensurePos() => $_ensure(0);

  @$pb.TagNumber(2)
  Quat get rot => $_getN(1);
  @$pb.TagNumber(2)
  set rot(Quat v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasRot() => $_has(1);
  @$pb.TagNumber(2)
  void clearRot() => clearField(2);
  @$pb.TagNumber(2)
  Quat ensureRot() => $_ensure(1);
}

class DriverMatrix4 extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'DriverMatrix4', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..a<$core.double>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm00', $pb.PbFieldType.OF)
    ..a<$core.double>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm01', $pb.PbFieldType.OF)
    ..a<$core.double>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm02', $pb.PbFieldType.OF)
    ..a<$core.double>(4, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm03', $pb.PbFieldType.OF)
    ..a<$core.double>(5, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm10', $pb.PbFieldType.OF)
    ..a<$core.double>(6, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm11', $pb.PbFieldType.OF)
    ..a<$core.double>(7, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm12', $pb.PbFieldType.OF)
    ..a<$core.double>(8, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm13', $pb.PbFieldType.OF)
    ..a<$core.double>(9, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm20', $pb.PbFieldType.OF)
    ..a<$core.double>(10, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm21', $pb.PbFieldType.OF)
    ..a<$core.double>(11, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm22', $pb.PbFieldType.OF)
    ..a<$core.double>(12, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'm23', $pb.PbFieldType.OF)
    ..hasRequiredFields = false
  ;

  DriverMatrix4._() : super();
  factory DriverMatrix4({
    $core.double? m00,
    $core.double? m01,
    $core.double? m02,
    $core.double? m03,
    $core.double? m10,
    $core.double? m11,
    $core.double? m12,
    $core.double? m13,
    $core.double? m20,
    $core.double? m21,
    $core.double? m22,
    $core.double? m23,
  }) {
    final _result = create();
    if (m00 != null) {
      _result.m00 = m00;
    }
    if (m01 != null) {
      _result.m01 = m01;
    }
    if (m02 != null) {
      _result.m02 = m02;
    }
    if (m03 != null) {
      _result.m03 = m03;
    }
    if (m10 != null) {
      _result.m10 = m10;
    }
    if (m11 != null) {
      _result.m11 = m11;
    }
    if (m12 != null) {
      _result.m12 = m12;
    }
    if (m13 != null) {
      _result.m13 = m13;
    }
    if (m20 != null) {
      _result.m20 = m20;
    }
    if (m21 != null) {
      _result.m21 = m21;
    }
    if (m22 != null) {
      _result.m22 = m22;
    }
    if (m23 != null) {
      _result.m23 = m23;
    }
    return _result;
  }
  factory DriverMatrix4.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory DriverMatrix4.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  DriverMatrix4 clone() => DriverMatrix4()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  DriverMatrix4 copyWith(void Function(DriverMatrix4) updates) => super.copyWith((message) => updates(message as DriverMatrix4)) as DriverMatrix4; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static DriverMatrix4 create() => DriverMatrix4._();
  DriverMatrix4 createEmptyInstance() => create();
  static $pb.PbList<DriverMatrix4> createRepeated() => $pb.PbList<DriverMatrix4>();
  @$core.pragma('dart2js:noInline')
  static DriverMatrix4 getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<DriverMatrix4>(create);
  static DriverMatrix4? _defaultInstance;

  @$pb.TagNumber(1)
  $core.double get m00 => $_getN(0);
  @$pb.TagNumber(1)
  set m00($core.double v) { $_setFloat(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasM00() => $_has(0);
  @$pb.TagNumber(1)
  void clearM00() => clearField(1);

  @$pb.TagNumber(2)
  $core.double get m01 => $_getN(1);
  @$pb.TagNumber(2)
  set m01($core.double v) { $_setFloat(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasM01() => $_has(1);
  @$pb.TagNumber(2)
  void clearM01() => clearField(2);

  @$pb.TagNumber(3)
  $core.double get m02 => $_getN(2);
  @$pb.TagNumber(3)
  set m02($core.double v) { $_setFloat(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasM02() => $_has(2);
  @$pb.TagNumber(3)
  void clearM02() => clearField(3);

  @$pb.TagNumber(4)
  $core.double get m03 => $_getN(3);
  @$pb.TagNumber(4)
  set m03($core.double v) { $_setFloat(3, v); }
  @$pb.TagNumber(4)
  $core.bool hasM03() => $_has(3);
  @$pb.TagNumber(4)
  void clearM03() => clearField(4);

  @$pb.TagNumber(5)
  $core.double get m10 => $_getN(4);
  @$pb.TagNumber(5)
  set m10($core.double v) { $_setFloat(4, v); }
  @$pb.TagNumber(5)
  $core.bool hasM10() => $_has(4);
  @$pb.TagNumber(5)
  void clearM10() => clearField(5);

  @$pb.TagNumber(6)
  $core.double get m11 => $_getN(5);
  @$pb.TagNumber(6)
  set m11($core.double v) { $_setFloat(5, v); }
  @$pb.TagNumber(6)
  $core.bool hasM11() => $_has(5);
  @$pb.TagNumber(6)
  void clearM11() => clearField(6);

  @$pb.TagNumber(7)
  $core.double get m12 => $_getN(6);
  @$pb.TagNumber(7)
  set m12($core.double v) { $_setFloat(6, v); }
  @$pb.TagNumber(7)
  $core.bool hasM12() => $_has(6);
  @$pb.TagNumber(7)
  void clearM12() => clearField(7);

  @$pb.TagNumber(8)
  $core.double get m13 => $_getN(7);
  @$pb.TagNumber(8)
  set m13($core.double v) { $_setFloat(7, v); }
  @$pb.TagNumber(8)
  $core.bool hasM13() => $_has(7);
  @$pb.TagNumber(8)
  void clearM13() => clearField(8);

  @$pb.TagNumber(9)
  $core.double get m20 => $_getN(8);
  @$pb.TagNumber(9)
  set m20($core.double v) { $_setFloat(8, v); }
  @$pb.TagNumber(9)
  $core.bool hasM20() => $_has(8);
  @$pb.TagNumber(9)
  void clearM20() => clearField(9);

  @$pb.TagNumber(10)
  $core.double get m21 => $_getN(9);
  @$pb.TagNumber(10)
  set m21($core.double v) { $_setFloat(9, v); }
  @$pb.TagNumber(10)
  $core.bool hasM21() => $_has(9);
  @$pb.TagNumber(10)
  void clearM21() => clearField(10);

  @$pb.TagNumber(11)
  $core.double get m22 => $_getN(10);
  @$pb.TagNumber(11)
  set m22($core.double v) { $_setFloat(10, v); }
  @$pb.TagNumber(11)
  $core.bool hasM22() => $_has(10);
  @$pb.TagNumber(11)
  void clearM22() => clearField(11);

  @$pb.TagNumber(12)
  $core.double get m23 => $_getN(11);
  @$pb.TagNumber(12)
  set m23($core.double v) { $_setFloat(11, v); }
  @$pb.TagNumber(12)
  $core.bool hasM23() => $_has(11);
  @$pb.TagNumber(12)
  void clearM23() => clearField(12);
}

class DriverPose extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'DriverPose', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<DriverMatrix4>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'pose', subBuilder: DriverMatrix4.create)
    ..a<$fixnum.Int64>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'ts', $pb.PbFieldType.OU6, defaultOrMaker: $fixnum.Int64.ZERO)
    ..hasRequiredFields = false
  ;

  DriverPose._() : super();
  factory DriverPose({
    DriverMatrix4? pose,
    $fixnum.Int64? ts,
  }) {
    final _result = create();
    if (pose != null) {
      _result.pose = pose;
    }
    if (ts != null) {
      _result.ts = ts;
    }
    return _result;
  }
  factory DriverPose.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory DriverPose.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  DriverPose clone() => DriverPose()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  DriverPose copyWith(void Function(DriverPose) updates) => super.copyWith((message) => updates(message as DriverPose)) as DriverPose; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static DriverPose create() => DriverPose._();
  DriverPose createEmptyInstance() => create();
  static $pb.PbList<DriverPose> createRepeated() => $pb.PbList<DriverPose>();
  @$core.pragma('dart2js:noInline')
  static DriverPose getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<DriverPose>(create);
  static DriverPose? _defaultInstance;

  @$pb.TagNumber(1)
  DriverMatrix4 get pose => $_getN(0);
  @$pb.TagNumber(1)
  set pose(DriverMatrix4 v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPose() => $_has(0);
  @$pb.TagNumber(1)
  void clearPose() => clearField(1);
  @$pb.TagNumber(1)
  DriverMatrix4 ensurePose() => $_ensure(0);

  @$pb.TagNumber(2)
  $fixnum.Int64 get ts => $_getI64(1);
  @$pb.TagNumber(2)
  set ts($fixnum.Int64 v) { $_setInt64(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasTs() => $_has(1);
  @$pb.TagNumber(2)
  void clearTs() => clearField(2);
}

class K4ABTPose extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'K4ABTPose', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<Pose>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'pose', subBuilder: Pose.create)
    ..a<$fixnum.Int64>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'devTs', $pb.PbFieldType.OU6, defaultOrMaker: $fixnum.Int64.ZERO)
    ..a<$fixnum.Int64>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'sysTs', $pb.PbFieldType.OU6, defaultOrMaker: $fixnum.Int64.ZERO)
    ..hasRequiredFields = false
  ;

  K4ABTPose._() : super();
  factory K4ABTPose({
    Pose? pose,
    $fixnum.Int64? devTs,
    $fixnum.Int64? sysTs,
  }) {
    final _result = create();
    if (pose != null) {
      _result.pose = pose;
    }
    if (devTs != null) {
      _result.devTs = devTs;
    }
    if (sysTs != null) {
      _result.sysTs = sysTs;
    }
    return _result;
  }
  factory K4ABTPose.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory K4ABTPose.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  K4ABTPose clone() => K4ABTPose()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  K4ABTPose copyWith(void Function(K4ABTPose) updates) => super.copyWith((message) => updates(message as K4ABTPose)) as K4ABTPose; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static K4ABTPose create() => K4ABTPose._();
  K4ABTPose createEmptyInstance() => create();
  static $pb.PbList<K4ABTPose> createRepeated() => $pb.PbList<K4ABTPose>();
  @$core.pragma('dart2js:noInline')
  static K4ABTPose getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<K4ABTPose>(create);
  static K4ABTPose? _defaultInstance;

  @$pb.TagNumber(1)
  Pose get pose => $_getN(0);
  @$pb.TagNumber(1)
  set pose(Pose v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPose() => $_has(0);
  @$pb.TagNumber(1)
  void clearPose() => clearField(1);
  @$pb.TagNumber(1)
  Pose ensurePose() => $_ensure(0);

  @$pb.TagNumber(2)
  $fixnum.Int64 get devTs => $_getI64(1);
  @$pb.TagNumber(2)
  set devTs($fixnum.Int64 v) { $_setInt64(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasDevTs() => $_has(1);
  @$pb.TagNumber(2)
  void clearDevTs() => clearField(2);

  @$pb.TagNumber(3)
  $fixnum.Int64 get sysTs => $_getI64(2);
  @$pb.TagNumber(3)
  set sysTs($fixnum.Int64 v) { $_setInt64(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasSysTs() => $_has(2);
  @$pb.TagNumber(3)
  void clearSysTs() => clearField(3);
}

class ImuSample extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'ImuSample', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<Vec3>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'sample', subBuilder: Vec3.create)
    ..a<$fixnum.Int64>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'ts', $pb.PbFieldType.OU6, defaultOrMaker: $fixnum.Int64.ZERO)
    ..hasRequiredFields = false
  ;

  ImuSample._() : super();
  factory ImuSample({
    Vec3? sample,
    $fixnum.Int64? ts,
  }) {
    final _result = create();
    if (sample != null) {
      _result.sample = sample;
    }
    if (ts != null) {
      _result.ts = ts;
    }
    return _result;
  }
  factory ImuSample.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory ImuSample.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  ImuSample clone() => ImuSample()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  ImuSample copyWith(void Function(ImuSample) updates) => super.copyWith((message) => updates(message as ImuSample)) as ImuSample; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static ImuSample create() => ImuSample._();
  ImuSample createEmptyInstance() => create();
  static $pb.PbList<ImuSample> createRepeated() => $pb.PbList<ImuSample>();
  @$core.pragma('dart2js:noInline')
  static ImuSample getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<ImuSample>(create);
  static ImuSample? _defaultInstance;

  @$pb.TagNumber(1)
  Vec3 get sample => $_getN(0);
  @$pb.TagNumber(1)
  set sample(Vec3 v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasSample() => $_has(0);
  @$pb.TagNumber(1)
  void clearSample() => clearField(1);
  @$pb.TagNumber(1)
  Vec3 ensureSample() => $_ensure(0);

  @$pb.TagNumber(2)
  $fixnum.Int64 get ts => $_getI64(1);
  @$pb.TagNumber(2)
  set ts($fixnum.Int64 v) { $_setInt64(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasTs() => $_has(1);
  @$pb.TagNumber(2)
  void clearTs() => clearField(2);
}

class CalibrationData extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'CalibrationData', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<DriverPose>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'hmd', subBuilder: DriverPose.create)
    ..aOM<K4ABTPose>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'head', subBuilder: K4ABTPose.create)
    ..hasRequiredFields = false
  ;

  CalibrationData._() : super();
  factory CalibrationData({
    DriverPose? hmd,
    K4ABTPose? head,
  }) {
    final _result = create();
    if (hmd != null) {
      _result.hmd = hmd;
    }
    if (head != null) {
      _result.head = head;
    }
    return _result;
  }
  factory CalibrationData.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory CalibrationData.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  CalibrationData clone() => CalibrationData()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  CalibrationData copyWith(void Function(CalibrationData) updates) => super.copyWith((message) => updates(message as CalibrationData)) as CalibrationData; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static CalibrationData create() => CalibrationData._();
  CalibrationData createEmptyInstance() => create();
  static $pb.PbList<CalibrationData> createRepeated() => $pb.PbList<CalibrationData>();
  @$core.pragma('dart2js:noInline')
  static CalibrationData getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<CalibrationData>(create);
  static CalibrationData? _defaultInstance;

  @$pb.TagNumber(1)
  DriverPose get hmd => $_getN(0);
  @$pb.TagNumber(1)
  set hmd(DriverPose v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasHmd() => $_has(0);
  @$pb.TagNumber(1)
  void clearHmd() => clearField(1);
  @$pb.TagNumber(1)
  DriverPose ensureHmd() => $_ensure(0);

  @$pb.TagNumber(2)
  K4ABTPose get head => $_getN(1);
  @$pb.TagNumber(2)
  set head(K4ABTPose v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasHead() => $_has(1);
  @$pb.TagNumber(2)
  void clearHead() => clearField(2);
  @$pb.TagNumber(2)
  K4ABTPose ensureHead() => $_ensure(1);
}

class CalibrationDataRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'CalibrationDataRequest', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  CalibrationDataRequest._() : super();
  factory CalibrationDataRequest() => create();
  factory CalibrationDataRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory CalibrationDataRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  CalibrationDataRequest clone() => CalibrationDataRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  CalibrationDataRequest copyWith(void Function(CalibrationDataRequest) updates) => super.copyWith((message) => updates(message as CalibrationDataRequest)) as CalibrationDataRequest; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static CalibrationDataRequest create() => CalibrationDataRequest._();
  CalibrationDataRequest createEmptyInstance() => create();
  static $pb.PbList<CalibrationDataRequest> createRepeated() => $pb.PbList<CalibrationDataRequest>();
  @$core.pragma('dart2js:noInline')
  static CalibrationDataRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<CalibrationDataRequest>(create);
  static CalibrationDataRequest? _defaultInstance;
}

class CalibrationDataReply extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'CalibrationDataReply', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOB(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'success')
    ..aOM<ImuSample>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'acc', subBuilder: ImuSample.create)
    ..aOM<ImuSample>(3, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'gyro', subBuilder: ImuSample.create)
    ..aOM<CalibrationData>(4, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'data', subBuilder: CalibrationData.create)
    ..hasRequiredFields = false
  ;

  CalibrationDataReply._() : super();
  factory CalibrationDataReply({
    $core.bool? success,
    ImuSample? acc,
    ImuSample? gyro,
    CalibrationData? data,
  }) {
    final _result = create();
    if (success != null) {
      _result.success = success;
    }
    if (acc != null) {
      _result.acc = acc;
    }
    if (gyro != null) {
      _result.gyro = gyro;
    }
    if (data != null) {
      _result.data = data;
    }
    return _result;
  }
  factory CalibrationDataReply.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory CalibrationDataReply.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  CalibrationDataReply clone() => CalibrationDataReply()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  CalibrationDataReply copyWith(void Function(CalibrationDataReply) updates) => super.copyWith((message) => updates(message as CalibrationDataReply)) as CalibrationDataReply; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static CalibrationDataReply create() => CalibrationDataReply._();
  CalibrationDataReply createEmptyInstance() => create();
  static $pb.PbList<CalibrationDataReply> createRepeated() => $pb.PbList<CalibrationDataReply>();
  @$core.pragma('dart2js:noInline')
  static CalibrationDataReply getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<CalibrationDataReply>(create);
  static CalibrationDataReply? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get success => $_getBF(0);
  @$pb.TagNumber(1)
  set success($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasSuccess() => $_has(0);
  @$pb.TagNumber(1)
  void clearSuccess() => clearField(1);

  @$pb.TagNumber(2)
  ImuSample get acc => $_getN(1);
  @$pb.TagNumber(2)
  set acc(ImuSample v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasAcc() => $_has(1);
  @$pb.TagNumber(2)
  void clearAcc() => clearField(2);
  @$pb.TagNumber(2)
  ImuSample ensureAcc() => $_ensure(1);

  @$pb.TagNumber(3)
  ImuSample get gyro => $_getN(2);
  @$pb.TagNumber(3)
  set gyro(ImuSample v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasGyro() => $_has(2);
  @$pb.TagNumber(3)
  void clearGyro() => clearField(3);
  @$pb.TagNumber(3)
  ImuSample ensureGyro() => $_ensure(2);

  @$pb.TagNumber(4)
  CalibrationData get data => $_getN(3);
  @$pb.TagNumber(4)
  set data(CalibrationData v) { setField(4, v); }
  @$pb.TagNumber(4)
  $core.bool hasData() => $_has(3);
  @$pb.TagNumber(4)
  void clearData() => clearField(4);
  @$pb.TagNumber(4)
  CalibrationData ensureData() => $_ensure(3);
}

class UpdateCalibrationRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'UpdateCalibrationRequest', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOM<Quat>(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'qWorldFromDriverRotation', subBuilder: Quat.create)
    ..aOM<Vec3>(2, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'vecWorldFromDriverTranslation', subBuilder: Vec3.create)
    ..hasRequiredFields = false
  ;

  UpdateCalibrationRequest._() : super();
  factory UpdateCalibrationRequest({
    Quat? qWorldFromDriverRotation,
    Vec3? vecWorldFromDriverTranslation,
  }) {
    final _result = create();
    if (qWorldFromDriverRotation != null) {
      _result.qWorldFromDriverRotation = qWorldFromDriverRotation;
    }
    if (vecWorldFromDriverTranslation != null) {
      _result.vecWorldFromDriverTranslation = vecWorldFromDriverTranslation;
    }
    return _result;
  }
  factory UpdateCalibrationRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory UpdateCalibrationRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  UpdateCalibrationRequest clone() => UpdateCalibrationRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  UpdateCalibrationRequest copyWith(void Function(UpdateCalibrationRequest) updates) => super.copyWith((message) => updates(message as UpdateCalibrationRequest)) as UpdateCalibrationRequest; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static UpdateCalibrationRequest create() => UpdateCalibrationRequest._();
  UpdateCalibrationRequest createEmptyInstance() => create();
  static $pb.PbList<UpdateCalibrationRequest> createRepeated() => $pb.PbList<UpdateCalibrationRequest>();
  @$core.pragma('dart2js:noInline')
  static UpdateCalibrationRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<UpdateCalibrationRequest>(create);
  static UpdateCalibrationRequest? _defaultInstance;

  @$pb.TagNumber(1)
  Quat get qWorldFromDriverRotation => $_getN(0);
  @$pb.TagNumber(1)
  set qWorldFromDriverRotation(Quat v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasQWorldFromDriverRotation() => $_has(0);
  @$pb.TagNumber(1)
  void clearQWorldFromDriverRotation() => clearField(1);
  @$pb.TagNumber(1)
  Quat ensureQWorldFromDriverRotation() => $_ensure(0);

  @$pb.TagNumber(2)
  Vec3 get vecWorldFromDriverTranslation => $_getN(1);
  @$pb.TagNumber(2)
  set vecWorldFromDriverTranslation(Vec3 v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasVecWorldFromDriverTranslation() => $_has(1);
  @$pb.TagNumber(2)
  void clearVecWorldFromDriverTranslation() => clearField(2);
  @$pb.TagNumber(2)
  Vec3 ensureVecWorldFromDriverTranslation() => $_ensure(1);
}

class UpdateCalibrationReply extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'UpdateCalibrationReply', package: const $pb.PackageName(const $core.bool.fromEnvironment('protobuf.omit_message_names') ? '' : 'calibration'), createEmptyInstance: create)
    ..aOB(1, const $core.bool.fromEnvironment('protobuf.omit_field_names') ? '' : 'success')
    ..hasRequiredFields = false
  ;

  UpdateCalibrationReply._() : super();
  factory UpdateCalibrationReply({
    $core.bool? success,
  }) {
    final _result = create();
    if (success != null) {
      _result.success = success;
    }
    return _result;
  }
  factory UpdateCalibrationReply.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory UpdateCalibrationReply.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  UpdateCalibrationReply clone() => UpdateCalibrationReply()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  UpdateCalibrationReply copyWith(void Function(UpdateCalibrationReply) updates) => super.copyWith((message) => updates(message as UpdateCalibrationReply)) as UpdateCalibrationReply; // ignore: deprecated_member_use
  $pb.BuilderInfo get info_ => _i;
  @$core.pragma('dart2js:noInline')
  static UpdateCalibrationReply create() => UpdateCalibrationReply._();
  UpdateCalibrationReply createEmptyInstance() => create();
  static $pb.PbList<UpdateCalibrationReply> createRepeated() => $pb.PbList<UpdateCalibrationReply>();
  @$core.pragma('dart2js:noInline')
  static UpdateCalibrationReply getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<UpdateCalibrationReply>(create);
  static UpdateCalibrationReply? _defaultInstance;

  @$pb.TagNumber(1)
  $core.bool get success => $_getBF(0);
  @$pb.TagNumber(1)
  set success($core.bool v) { $_setBool(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasSuccess() => $_has(0);
  @$pb.TagNumber(1)
  void clearSuccess() => clearField(1);
}

