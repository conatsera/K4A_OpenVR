import 'package:flutter/material.dart';

import 'services.dart';
import 'generated/calibration.pbgrpc.dart';

import 'scaffolding.dart';
import 'calibrator.dart';

final kDefaultSettings = SettingsUpdateRequest(
  mode: SettingsUpdateRequest_ProcessingMode.DEFAULT,
  lite: false,
  smoothing: 0.1,
  depthRes: SettingsUpdateRequest_DepthResolution.NFOV_UNBINNED,
);

Future<SettingsUpdateRequest>? currentSettingsFuture;

void main() {
  runApp(const Splash());

  settingsClient.get(SettingsGetRequest()).then((r) {
    kDefaultSettings.depthRes = r.depthRes;
    kDefaultSettings.lite = r.lite;
    kDefaultSettings.mode = r.mode;
    kDefaultSettings.smoothing = r.smoothing;
  }).catchError((e) {
    return null;
  }).whenComplete(() => runApp(const MyApp()));
}

class Splash extends StatelessWidget {
  const Splash({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'K4A_OpenVR Settings',
      theme: ThemeData(
        primarySwatch: Colors.deepPurple,
      ),
      home: Scaffold(
        drawer: Drawer(
            child: ListView(
          children: const [
            ListTile(
              title: Text('Settings'),
            ),
            ListTile(
              title: Text('Calibrator'),
            ),
          ],
        )),
        appBar: AppBar(
          title: const Text('Settings'),
        ),
        body: const Center(
          child: CircularProgressIndicator(),
        ),
        bottomNavigationBar: const BottomAppBarControls(),
      ),
      onGenerateRoute: (settings) {
        return PageRouteBuilder(
          settings: settings,
          pageBuilder: (_, __, ___) => Scaffold(
            drawer: Drawer(
                child: ListView(
              children: const [
                ListTile(
                  title: Text('Settings'),
                ),
                ListTile(
                  title: Text('Calibrator'),
                ),
              ],
            )),
            appBar: AppBar(
              title: const Text('Settings'),
            ),
            body: const Center(
              child: CircularProgressIndicator(),
            ),
            bottomNavigationBar: const BottomAppBarControls(),
          ),
          transitionsBuilder: (_, a, __, c) =>
              FadeTransition(opacity: a, child: c),
          transitionDuration: const Duration(milliseconds: 100),
        );
      },
    );
  }
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'K4A_OpenVR Settings',
      theme: ThemeData(
        primarySwatch: Colors.deepPurple,
      ),
      home: const SettingsPage(),
      onGenerateRoute: (settings) {
        switch (settings.name) {
          case '/calibrator':
            return PageRouteBuilder(
              settings: settings,
              pageBuilder: (_, __, ___) => const CalibratorPage(),
              transitionsBuilder: (_, a, __, c) =>
                  FadeTransition(opacity: a, child: c),
              transitionDuration: const Duration(milliseconds: 100),
            );
          default:
            return PageRouteBuilder(
              settings: settings,
              pageBuilder: (_, __, ___) => const SettingsPage(),
              transitionsBuilder: (_, a, __, c) =>
                  FadeTransition(opacity: a, child: c),
              transitionDuration: const Duration(milliseconds: 100),
            );
        }
      },
    );
  }
}

class SettingsPage extends StatefulWidget {
  const SettingsPage({Key? key}) : super(key: key);

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

bool _isValid = false;

class _SettingsPageState extends State<SettingsPage> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  final SettingsUpdateRequest req = kDefaultSettings;

  void _openSettings() {
    Navigator.pop(context);
  }

  void _openCalibrator() {
    Navigator.popAndPushNamed(context, '/calibrator');
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
      )),
      appBar: AppBar(
        title: const Text('Settings'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Form(
              key: _formKey,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                children: <Widget>[
                  Row(mainAxisAlignment: MainAxisAlignment.start, children: [
                    const Padding(
                      padding: EdgeInsets.all(16.0),
                      child: Text(
                        'Lite Mode: ',
                        style: TextStyle(fontSize: 16.0),
                      ),
                    ),
                    Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: FormField<bool>(
                        initialValue: false,
                        builder: (FormFieldState<bool> state) {
                          return Switch(
                            onChanged: (value) {
                              setState(() {
                                req.lite = value;
                              });
                              state.didChange(value);
                            },
                            value: req.lite,
                          );
                        },
                      ),
                    )
                  ]),
                  Row(mainAxisAlignment: MainAxisAlignment.center, children: [
                    const Padding(
                      padding: EdgeInsets.all(16.0),
                      child: Text(
                        'Processing Mode: ',
                        style: TextStyle(fontSize: 16.0),
                      ),
                    ),
                    Expanded(
                      child: Padding(
                        padding: const EdgeInsets.all(32.0),
                        child: DropdownButtonFormField<
                                SettingsUpdateRequest_ProcessingMode>(
                            decoration: const InputDecoration(
                              hintText: 'Select processing mode',
                            ),
                            validator:
                                (SettingsUpdateRequest_ProcessingMode? value) {
                              if (value == null) {
                                return 'Requires value';
                              }
                              return null;
                            },
                            onChanged:
                                (SettingsUpdateRequest_ProcessingMode? value) {
                              if (value != null) {
                                setState(() {
                                  req.mode = value;
                                });
                              }
                            },
                            items: SettingsUpdateRequest_ProcessingMode.values.map<
                                    DropdownMenuItem<
                                        SettingsUpdateRequest_ProcessingMode>>(
                                (SettingsUpdateRequest_ProcessingMode value) {
                              return DropdownMenuItem<
                                  SettingsUpdateRequest_ProcessingMode>(
                                value: value,
                                child: Text(
                                  value.name,
                                ),
                              );
                            }).toList(),
                            value: req.mode),
                      ),
                    ),
                  ]),
                  Row(mainAxisAlignment: MainAxisAlignment.center, children: [
                    const Padding(
                      padding: EdgeInsets.all(16.0),
                      child: Text(
                        'Depth Resolution: ',
                        style: TextStyle(fontSize: 16.0),
                      ),
                    ),
                    Expanded(
                      child: Padding(
                        padding: const EdgeInsets.all(32.0),
                        child: DropdownButtonFormField<
                                SettingsUpdateRequest_DepthResolution>(
                            decoration: const InputDecoration(
                              hintText: 'Select depth resolution mode',
                            ),
                            validator:
                                (SettingsUpdateRequest_DepthResolution? value) {
                              if (value == null) {
                                return 'Requires value';
                              }
                              return null;
                            },
                            onChanged:
                                (SettingsUpdateRequest_DepthResolution? value) {
                              if (value != null) {
                                setState(() {
                                  req.depthRes = value;
                                });
                              }
                            },
                            items: SettingsUpdateRequest_DepthResolution.values.map<
                                    DropdownMenuItem<
                                        SettingsUpdateRequest_DepthResolution>>(
                                (SettingsUpdateRequest_DepthResolution value) {
                              return DropdownMenuItem<
                                  SettingsUpdateRequest_DepthResolution>(
                                value: value,
                                child: Text(
                                  value.name,
                                ),
                              );
                            }).toList(),
                            value: req.depthRes),
                      ),
                    ),
                  ]),
                  Row(mainAxisAlignment: MainAxisAlignment.center, children: [
                    const Padding(
                      padding: EdgeInsets.all(16.0),
                      child: Text(
                        'Smoothing: ',
                        style: TextStyle(fontSize: 16.0),
                      ),
                    ),
                    Expanded(
                      child: Padding(
                        padding: const EdgeInsets.all(16.0),
                        child: FormField<double>(
                            initialValue: 0.1,
                            builder: (FormFieldState<double> state) {
                              return Slider(
                                onChanged: (double? value) {
                                  if (value != null) {
                                    setState(() {
                                      req.smoothing = value;
                                    });
                                  }
                                  state.didChange(value);
                                },
                                value: req.smoothing,
                                min: 0.0,
                                max: 1.0,
                                divisions: 20,
                                label: (req.smoothing.toString()),
                              );
                            }),
                      ),
                    ),
                  ]),
                ],
              ),
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          if (_formKey.currentState!.validate()) {
            // Process data
            final response = settingsClient.update(req).then((r) {
              if (r.success) {
                ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
                  content: Text(
                    'Update success',
                  ),
                ));
              } else {
                ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
                  content: Text(
                    'Update failed',
                    style: TextStyle(color: Colors.red),
                  ),
                ));
              }
            }).catchError((e) => handleGrpcError(context, e));
          }
        },
        tooltip: 'Update',
        child: const Icon(Icons.update),
      ),
      bottomNavigationBar: const BottomAppBarControls(),
    );
  }
}
