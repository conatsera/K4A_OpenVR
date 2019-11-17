# K4A_OpenVR
The calibrator is a temporary solution and currently requires sensor placement to be on either the X or Z axis.

## CMake variables
Set the `Qt5_DIR` variable to your Qt build platform (ex: `D:/Qt/5.12.5/msvc2017_64/lib/cmake/Qt5` )

`K4A_SDK_ROOT` and `K4ABT_ROOT` may be overridden for custom install directories. Left unset it will search the default install directory for Windows.

`REDIST` variable to `TRUE` to copy necessary redistributables to the output.

Set `CMAKE_INSTALL_PREFIX` to where you want the driver to be installed (a `k4a_openvr` directory will be created here)

## Driver setup

Ensure CUDa, CUDNN, K4A SDK, and K4ABT binaries are in your system path or set the `REDIST` variable to `TRUE`

Copy `dnn_model_2_0.onnx` into `Steam\steamapps\common\SteamVR`

Run `cmake --install`