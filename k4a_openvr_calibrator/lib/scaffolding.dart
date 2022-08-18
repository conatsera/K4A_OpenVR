import 'package:flutter/material.dart';

import 'services.dart';
import 'generated/calibration.pbgrpc.dart';

class PauseResumeButton extends StatelessWidget {
  const PauseResumeButton(
      {Key? key, required this.onPressed, this.paused = false})
      : super(key: key);

  final bool paused;

  final VoidCallback? onPressed;

  @override
  @override
  Widget build(BuildContext context) {
    final icon = paused ? Icons.play_arrow : Icons.pause;
    return IconButton(
      icon: Icon(icon),
      onPressed: onPressed,
    );
  }
}

class BottomAppBarControls extends StatefulWidget {
  const BottomAppBarControls({Key? key}) : super(key: key);

  @override
  State<BottomAppBarControls> createState() => BottomAppBarControlsState();
}

bool _pauseState = false;

class BottomAppBarControlsState extends State<BottomAppBarControls> {
  void _handlePauseStateChanged(bool newPauseState) {
    setState(() {
      _pauseState = newPauseState;
    });
  }

  @override
  Widget build(BuildContext context) {
    return BottomAppBar(
        color: Colors.deepPurpleAccent,
        child: ButtonBar(
          alignment: MainAxisAlignment.end,
          children: [
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 16.0),
              child: PauseResumeButton(
                paused: _pauseState,
                onPressed: () {
                  final response = settingsClient
                      .pauseResume(PauseResumeRequest())
                      .then((r) {
                    _handlePauseStateChanged(r.paused);
                  }).catchError((e) => handleGrpcError(context, e));
                },
              ),
            ),
          ],
        ));
  }
}
