cmake_minimum_required(VERSION 3.14)
project(k4a_openvr_calibrator LANGUAGES CXX)

set(BINARY_NAME "k4a_openvr_calibrator")

cmake_policy(SET CMP0063 NEW)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

# Configure build options.
get_property(IS_MULTICONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTICONFIG)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Profile;Release"
    CACHE STRING "" FORCE)
else()
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Flutter build mode" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      "Debug" "Profile" "Release")
  endif()
endif()

set(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELEASE}")

# Use Unicode for all projects.
add_definitions(-DUNICODE -D_UNICODE)

# Compilation settings that should be applied to most targets.
function(APPLY_STANDARD_SETTINGS TARGET)
  target_compile_features(${TARGET} PUBLIC cxx_std_17)
  target_compile_options(${TARGET} PRIVATE /W4 /WX /wd"4100")
  target_compile_options(${TARGET} PRIVATE /EHsc)
  target_compile_definitions(${TARGET} PRIVATE "_HAS_EXCEPTIONS=0")
  target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:_DEBUG>")
endfunction()

set(FLUTTER_MANAGED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/flutter")

# Flutter library and tool build rules.
add_subdirectory(${FLUTTER_MANAGED_DIR})

# Application build
add_subdirectory("runner")

# Generated plugin build rules, which manage building the plugins and adding
# them to the application.
include(flutter/generated_plugins.cmake)


# === Installation ===
# Support files are copied into place next to the executable, so that it can
# run in place. This is done instead of making a separate bundle (as on Linux)
# so that building and running from within Visual Studio will work.
set(BUILD_BUNDLE_DIR "$<TARGET_FILE_DIR:${BINARY_NAME}>")
# Make the "install" step default, as it's required to run.
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${BUILD_BUNDLE_DIR}" CACHE PATH "..." FORCE)
endif()

set(INSTALL_BUNDLE_DATA_DIR "${CMAKE_INSTALL_PREFIX}/data")
set(INSTALL_BUNDLE_LIB_DIR "${CMAKE_INSTALL_PREFIX}")

install(TARGETS ${BINARY_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}"
  COMPONENT Runtime)

install(FILES "${FLUTTER_ICU_DATA_FILE}" DESTINATION "${INSTALL_BUNDLE_DATA_DIR}"
  COMPONENT Runtime)

install(FILES "${FLUTTER_LIBRARY}" DESTINATION "${INSTALL_BUNDLE_LIB_DIR}"
  COMPONENT Runtime)

if(PLUGIN_BUNDLED_LIBRARIES)
  install(FILES "${PLUGIN_BUNDLED_LIBRARIES}"
    DESTINATION "${INSTALL_BUNDLE_LIB_DIR}"
    COMPONENT Runtime)
endif()

# Fully re-copy the assets directory on each build to avoid having stale files
# from a previous install.
set(FLUTTER_ASSET_DIR_NAME "flutter_assets")
install(CODE "
  file(REMOVE_RECURSE \"${INSTALL_BUNDLE_DATA_DIR}/${FLUTTER_ASSET_DIR_NAME}\")
  " COMPONENT Runtime)
install(DIRECTORY "${PROJECT_BUILD_DIR}/${FLUTTER_ASSET_DIR_NAME}"
  DESTINATION "${INSTALL_BUNDLE_DATA_DIR}" COMPONENT Runtime)

# Install the AOT library on non-Debug builds only.
install(FILES "${AOT_LIBRARY}" DESTINATION "${INSTALL_BUNDLE_DATA_DIR}"
  CONFIGURATIONS Profile;Release
  COMPONENT Runtime)

# Calibration and openvr utility lib

if (NOT calibration_grpc_hdrs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../extern ${CMAKE_CURRENT_BINARY_DIR}/extern)
endif()
set(OPENVR_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../extern/openvr/src")
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

add_library(calibrator SHARED calibrator.cc)
target_include_directories(calibrator PUBLIC
  ${OPENVR_INCLUDE_DIR}
  ${Eigen_SOURCE_DIR}
)
target_link_libraries(calibrator ${OPENVR_LIBRARIES} Eigen3::Eigen)

target_compile_features(calibrator PUBLIC cxx_std_20)
target_compile_options(calibrator PUBLIC /ZH:SHA_256 /W4 /permissive- /sdl /WX /wd4100 /Wv:18 /wd4127 /bigobj)

add_executable(calibrator_test calibrator_test.cc)
target_include_directories(calibrator_test PUBLIC
  ${OPENVR_INCLUDE_DIR}
  ${Eigen_SOURCE_DIR}
)
target_link_libraries(calibrator_test ${OPENVR_LIBRARIES} Eigen3::Eigen)

target_compile_features(calibrator_test PUBLIC cxx_std_20)
target_compile_options(calibrator_test PUBLIC /ZH:SHA_256 /W4 /permissive- /sdl /WX /wd4100 /Wv:18 /wd4127 /bigobj)

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

install(FILES ${OPENVR_DLL} DESTINATION "${INSTALL_BUNDLE_LIB_DIR}"
  COMPONENT Runtime)