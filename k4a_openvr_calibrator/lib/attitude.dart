import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

class AttitudeIndicator extends StatefulWidget {
  const AttitudeIndicator({
    Key? key,
    this.yaw,
    this.pitch,
    this.roll,
    this.backgroundColor,
    this.color,
  }) : super(key: key);

  final double? yaw;
  final double? pitch;
  final double? roll;

  final Color? backgroundColor;
  final Color? color;

  @override
  void debugFillProperties(DiagnosticPropertiesBuilder properties) {
    super.debugFillProperties(properties);
    properties.add(
        DoubleProperty('yaw', yaw, showName: false, ifNull: '<indeterminate>'));
    properties.add(DoubleProperty('pitch', pitch,
        showName: false, ifNull: '<indeterminate>'));
    properties.add(DoubleProperty('roll', roll,
        showName: false, ifNull: '<indeterminate>'));
  }

  @override
  State<AttitudeIndicator> createState() => _AttitudeIndicatorState();
}

class _AttitudeIndicatorState extends State<AttitudeIndicator>
    with SingleTickerProviderStateMixin {
  Widget _buildAngleIndicator(BuildContext context, double angleValue) {
    final Color backgroundColor = widget.backgroundColor ??
        ProgressIndicatorTheme.of(context).circularTrackColor ??
        Colors.white;
    final Color color = widget.color ??
        ProgressIndicatorTheme.of(context).color ??
        Colors.black;
    return Container(
      constraints: const BoxConstraints(
        minWidth: 128.0,
        minHeight: 128.0,
      ),
      child: CustomPaint(
        painter: _AttitudeIndicatorPainter(
          backgroundColor: backgroundColor,
          color: color,
          value: angleValue,
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        _buildAngleIndicator(context, widget.yaw ?? 0.0),
        _buildAngleIndicator(context, widget.pitch ?? 0.0),
        _buildAngleIndicator(context, widget.roll ?? 0.0),
      ],
    );
  }
}

class _AttitudeIndicatorPainter extends CustomPainter {
  const _AttitudeIndicatorPainter({
    required this.backgroundColor,
    required this.color,
    required this.value,
  });

  final Color backgroundColor;
  final Color color;

  final double value;

  @override
  void paint(Canvas canvas, Size size) {
    final Paint paint = Paint()
      ..color = color
      ..strokeWidth = 4.0
      ..style = PaintingStyle.stroke;

    final anglePoint = Offset((64 * cos(value)) + size.width / 2,
        (64 * sin(value)) + size.height / 2);

    canvas.drawLine(Offset(size.width / 2, size.height / 2), anglePoint, paint);
  }

  @override
  bool shouldRepaint(_AttitudeIndicatorPainter oldPainter) {
    return oldPainter.backgroundColor != backgroundColor ||
        oldPainter.color != color ||
        oldPainter.value != value;
  }
}
