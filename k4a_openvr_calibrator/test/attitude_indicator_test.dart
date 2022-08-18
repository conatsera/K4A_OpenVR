import 'dart:io';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:k4a_openvr_calibrator/attitude.dart';
import 'package:k4a_openvr_calibrator/calibrator.dart';

import 'package:k4a_openvr_calibrator/main.dart';
import 'package:vector_math/vector_math.dart';

void main() {
  testWidgets('Spin attitude', (WidgetTester tester) async {
    double yaw = 0.0, pitch = 0.0, roll = 0.0;

    // Build our app and trigger a frame.
    await tester.pumpWidget(AttitudeIndicator(
      yaw: yaw,
      pitch: pitch,
      roll: roll,
    ));

    while (roll != pi * 4) {
      yaw += degrees2Radians * 1;
      pitch += degrees2Radians * 1;
      roll += degrees2Radians * 1;
      sleep(const Duration(milliseconds: 3));
    }

    // Tap the '+' icon and trigger a frame.
  });
}
