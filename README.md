# K4A_OpenVR
The calibrator is a temporary solution and currently requires sensor placement to be on either the X or Z axis.

## CMake variables
Set to `Release` build for max optimization settings.

`K4A_SDK_ROOT` and `K4ABT_ROOT` may be overridden for custom install directories. Left unset it will search the default install directory for Windows.

`REDIST` variable to `TRUE` to copy necessary redistributables to the output.

Set `CMAKE_INSTALL_PREFIX` to where you want the driver to be installed (a `k4a_openvr` directory will be created here)

## Driver setup

Ensure CUDa, CUDNN, K4A SDK, and K4ABT binaries are in your system path or set the `REDIST` variable to `TRUE`

Run `cmake --install`

## Calibration

Place the headset ontop of the kinect, facing the opposite direction, click calibrate.
I'm still working out the kinks, so while rotation appears to be spot on (at least in my tests), the position seems to shift a bit between launches of WMR.
Likely a result of a not great tracking scene behind my kinect, considering gyro would be unaffected by that. As such, position sliders are included to fix this.
The fine tuning mode is for rotation only and I don't advise its use currently. It's pretty broken and may be scrapped if Oculus Insight and SteamVR universes don't have any quirks that effect rotation either.
