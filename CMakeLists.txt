# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.14)

project ("K4A OpenVR" LANGUAGES CXX VERSION 0.9.9)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZH:SHA_256 /W4 /permissive- ")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
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
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory("extern")

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

include_directories("src")

# Include sub-projects.
add_subdirectory ("src")