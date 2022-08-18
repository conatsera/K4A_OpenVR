import 'dart:async';

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:grpc/grpc.dart';
import 'generated/calibration.pbgrpc.dart';

final channel = ClientChannel(
  'localhost',
  port: 51942,
  options: ChannelOptions(
    credentials: const ChannelCredentials.insecure(),
    codecRegistry: CodecRegistry(
      codecs: const [
        GzipCodec(),
        IdentityCodec(),
      ],
    ),
  ),
);

final settingsClient = SettingsClient(channel);
final calibrationClient = CalibrationClient(channel);

FutureOr<dynamic> handleGrpcError(BuildContext context, e) {
  ScaffoldMessenger.of(context).showSnackBar(SnackBar(
    content: Text(
      'gRPC Error: $e',
      style: const TextStyle(color: Colors.red),
    ),
  ));
}
