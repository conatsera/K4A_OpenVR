#[=======================================================================[.rst:
FindK4A
--------

Find K4A

Locates the K4A redistributables and libraries. Both SDK and body tracking
are searched
#]=======================================================================]
if (NOT DEFINED )
set(K4A_SDK_VERSION "v1.4.0")
endif()
if (NOT DEFINED ${K4A_SDK_ROOT})
find_path(K4A_SDK_ROOT
	  "Azure Kinect SDK ${K4A_SDK_VERSION}"
    PATHS
      ${PROJECT_SOURCE_DIR}
      /usr/share
      C:/
      "C:/Program Files"
      ${PROJECT_SOURCE_DIR}/..
      ENV ProgramFiles
    )
set(K4A_SDK_ROOT "${K4A_SDK_ROOT}/Azure Kinect SDK ${K4A_SDK_VERSION}")
endif()

if (NOT DEFINED ${K4ABT_ROOT})
find_path(K4ABT_ROOT
	  "Azure Kinect Body Tracking SDK"
    PATHS
      ${PROJECT_SOURCE_DIR}
      /usr/share
      C:/
      "C:/Program Files"
      ${PROJECT_SOURCE_DIR}/..
      ENV ProgramFiles
)
set(K4ABT_ROOT "${K4ABT_ROOT}/Azure Kinect Body Tracking SDK")
endif()

set(K4A_SDK_INCLUDE "${K4A_SDK_ROOT}/sdk/include")
set(K4ABT_SDK_INCLUDE "${K4ABT_ROOT}/sdk/include")

find_library(K4A_SDK
	NAMES
		k4a
	PATHS
		"${K4A_SDK_ROOT}/sdk/windows-desktop/amd64/release/lib"
		"${K4A_SDK_ROOT}/sdk/linux_clang/amd64/release/lib"
	NO_DEFAULT_PATH)

find_library(K4ABT_SDK
	NAMES
		k4abt
	PATHS
		"${K4ABT_ROOT}/sdk/windows-desktop/amd64/release/lib"
		"${K4ABT_ROOT}/lib" # TODO: Determine linux package structure
	NO_DEFAULT_PATH)

set(K4A_INCLUDE_DIRS "${K4A_SDK_INCLUDE}" "${K4ABT_SDK_INCLUDE}")
set(K4A_LIBRARIES "${K4A_SDK}" "${K4ABT_SDK}")

find_file(K4A_DLL
	NAMES
		k4a.dll
	PATHS
		"${K4A_SDK_ROOT}/sdk/windows-desktop/amd64/release/bin"
	NO_DEFAULT_PATH)

find_file(DEPTH_ENGINE_DLL
	NAMES
		depthengine_2_0.dll
	PATHS
		"${K4A_SDK_ROOT}/sdk/windows-desktop/amd64/release/bin"
	NO_DEFAULT_PATH)

find_file(K4ABT_DLL
	NAMES
		k4abt.dll
	PATHS
		"${K4ABT_ROOT}/sdk/windows-desktop/amd64/release/bin"
	NO_DEFAULT_PATH)

find_file(K4ABT_DNN
	"dnn_model_2_0.onnx"
	PATHS
		"${K4ABT_ROOT}/sdk/windows-desktop/amd64/release/bin"
	NO_DEFAULT_PATH)

find_file(ONNX_RT_DLL
	NAMES
		onnxruntime.dll
	PATHS
		"${K4ABT_ROOT}/sdk/windows-desktop/amd64/release/bin"
	NO_DEFAULT_PATH)