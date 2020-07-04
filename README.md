# K4A_OpenVR

This is a project that uses the Kinect Azure to simulate a hip and foot trackers for steam VR. It is still in early development so things may or may not work the way they are supposed to.

## CMake variables
Set to `Release` build for max optimization settings.

`K4A_SDK_ROOT` and `K4ABT_ROOT` may be overridden for custom install directories. Left unset it will search the default install directory for Windows.

Create `REDIST` variable and set to `TRUE` to copy necessary redistributables to the output.

Set `CMAKE_INSTALL_PREFIX` to where you want the driver to be installed (a `k4a_openvr` directory will be created here)

## Driver install

Ensure CUDa, CUDNN, K4A SDK, and K4ABT binaries are in your system path or set the `REDIST` variable to `TRUE`

Run `cmake --install` or install with your IDE

Move "dnn_model_2_0.onnx" from the azure body tracking sdk to the `k4a_openvr` directory
Move the `k4a_openvr` folder to "C:\Program Files "<your install directory>\Steam\steamapps\common\SteamVR\drivers"
NOTE:
For developers, it is easier to use "<your install directory>\Steam\steamapps\common\SteamVR\bin\win64\vrpathreg.exe" to add the `k4a_openvr` directory to steamVR's path instead of moving the folder over every new build.

When steamVR starts, the steam trackers should show up in settings. Make sure to set them to the correct roles.

## Calibration

Place the headset ontop of the kinect, facing the opposite direction, click calibrate.
I'm still working out the kinks, so while rotation appears to be spot on (at least in my tests), the position seems to shift a bit between launches of WMR.
Likely a result of a not great tracking scene behind my kinect, considering gyro would be unaffected by that.
The fine tuning mode is for rotation only and I don't advise its use currently. It's pretty broken and may be scrapped if Oculus Insight and SteamVR universes don't have any quirks that effect rotation either.

Testing with the Rift S lead to some funky position results. Calibrate button gets the trackers in view of the headset for testing purposes. 
