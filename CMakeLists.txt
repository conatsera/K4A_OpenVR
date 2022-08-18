# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)

project ("K4A OpenVR" LANGUAGES CXX VERSION 2.0)

cmake_policy(SET CMP0002 NEW)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries" FORCE)
set(gRPC_MSVC_STATIC_RUNTIME OFF CACHE BOOL "Link static runtime libraries" FORCE)
set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "Link static runtime libraries" FORCE)
set(CARES_MSVC_STATIC_RUNTIME OFF CACHE BOOL "Link static runtime libraries" FORCE)

if (WIN32)
	add_compile_definitions(_SILENCE_CXX20_CISO646_REMOVED_WARNING)
	if (NOT DEFINED GRPC_ROOT)
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(GRPC_ROOT "$ENV{ProgramFiles}/grpc-debug")
		else()
			set(GRPC_ROOT "$ENV{ProgramFiles}/grpc")
		endif()
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(_PROTOBUF_LIBPROTOBUF "${GRPC_ROOT}/lib/libprotobufd.lib")
	else()
		set(_PROTOBUF_LIBPROTOBUF "${GRPC_ROOT}/lib/libprotobuf.lib")
	endif()
	set(_GRPCPP_REFLECTION "${GRPC_ROOT}/lib/grpc++_reflection.lib")
	set(_GRPC_GRPCPP "${GRPC_ROOT}/lib/grpc++.lib")
	set(_ABSL_SYNC "${GRPC_ROOT}/lib/absl_synchronization.lib")

	set(GRPC_LIBS
		${_PROTOBUF_LIBPROTOBUF}
		${_GRPCPP_REFLECTION}
		${_GRPC_GRPCPP}
	)

	file(GLOB _ABSL_LIBS "${GRPC_ROOT}/lib/absl*")

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		list(APPEND GRPC_LIBS "${GRPC_ROOT}/lib/grpc++_unsecure.lib")
		list(APPEND GRPC_LIBS "${GRPC_ROOT}/lib/zlibstaticd.lib")
	else()
		list(FILTER _ABSL_LIBS EXCLUDE REGEX ".*lib/.+[Dd]ebug.*")
		
		list(APPEND GRPC_LIBS "${GRPC_ROOT}/lib/zlibstatic.lib")
	endif()

	list(APPEND GRPC_LIBS ${_ABSL_LIBS})

	list(APPEND GRPC_LIBS 
		"${GRPC_ROOT}/lib/grpc.lib"
		"${GRPC_ROOT}/lib/ssl.lib"
		"${GRPC_ROOT}/lib/crypto.lib"
		"${GRPC_ROOT}/lib/gpr.lib"
		"${GRPC_ROOT}/lib/cares.lib"
		"${GRPC_ROOT}/lib/re2.lib"
		"${GRPC_ROOT}/lib/upb.lib"
		"${GRPC_ROOT}/lib/address_sorting.lib"
	)

	set(_PROTOBUF_PROTOC "${GRPC_ROOT}/bin/protoc")
	set(_GRPC_CPP_PLUGIN_EXECUTABLE "${GRPC_ROOT}/bin/grpc_cpp_plugin.exe")

	include_directories("${GRPC_ROOT}/include")
else()
	include(FetchContent)
	FetchContent_Declare(
		gRPC
		GIT_REPOSITORY https://github.com/grpc/grpc
		GIT_TAG        v1.44.0
	)
	set(FETCHCONTENT_QUIET OFF)
	FetchContent_MakeAvailable(gRPC)

	set(_PROTOBUF_LIBPROTOBUF libprotobuf)
	set(_REFLECTION grpc++_reflection)
	set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
	set(_GRPC_GRPCPP grpc++)

	set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

get_filename_component(calibration_proto "protos/calibration.proto" ABSOLUTE)
get_filename_component(calibration_proto_path "${calibration_proto}" PATH)

# Generated sources
set(calibration_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/protos/calibration.pb.cc")
set(calibration_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/protos/calibration.pb.h")
set(calibration_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/protos/calibration.grpc.pb.cc")
set(calibration_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/protos/calibration.grpc.pb.h")

add_custom_command(
      OUTPUT "${calibration_proto_srcs}" "${calibration_proto_hdrs}" "${calibration_grpc_srcs}" "${calibration_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/protos"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/protos"
		--dart_out=grpc:"${CMAKE_CURRENT_SOURCE_DIR}/k4a_openvr_calibrator/lib/generated"
        -I "${calibration_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${calibration_proto}"
      DEPENDS "${calibration_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}/protos")

add_library(calibration_grpc_proto
  ${calibration_grpc_srcs}
  ${calibration_grpc_hdrs}
  ${calibration_proto_srcs}
  ${calibration_proto_hdrs})
target_link_libraries(calibration_grpc_proto
  ${GRPC_LIBS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZH:SHA_256 /W4 /permissive- /sdl /WX /wd4100 /Wv:18 /wd4127")
	add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ot /Oi /Ob3 /GL")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		# TODO: Add GCC optimizations
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
	endif()
endif()

add_definitions(-DUNICODE)

include("cmake/FindK4A.cmake")

add_subdirectory("extern")

#add_subdirectory("k4a_openvr_calibrator/windows")

enable_testing()

# OpenVR
set(OPENVR_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/openvr/src")
set(OPENVR_INCLUDE_DIR "${OPENVR_ROOT_DIR}/headers")

find_library(OPENVR_LIBRARIES
  NAMES
    openvr_api
  PATHS
    ${OPENVR_ROOT_DIR}/bin
    ${OPENVR_ROOT_DIR}/lib/win64
  PATH_SUFFIXES
    osx32
    linux64
    ${WINDOWS_PATH_SUFFIXES}
  NO_DEFAULT_PATH
)

find_file(OPENVR_DLL
	NAMES
		openvr_api.dll
	PATHS
		${OPENVR_ROOT_DIR}/bin
	PATH_SUFFIXES
		osx32
		linux64
		win64
	NO_DEFAULT_PATH
)
# End OpenVR

install(DIRECTORY DESTINATION k4a_openvr)

install(DIRECTORY DESTINATION k4a_openvr/bin/win64)

include_directories("src")

add_subdirectory("src")

add_subdirectory("tests")

install(FILES driver.vrdrivermanifest
	DESTINATION k4a_openvr)

#string(REPLACE "/" "\\\\\\\\" ESCAPED_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/k4a_openvr)

#get_filename_component(STEAM_VRPATHREG_EXE "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam;InstallPath]/steamapps/common/SteamVR/bin/win64/vrpathreg.exe" ABSOLUTE CACHE)

#install(CODE
#"execute_process(COMMAND \"${STEAM_VRPATHREG_EXE}\" adddriver \"${ESCAPED_INSTALL_PATH}\")")