cmake_minimum_required(VERSION 3.5)

project(calibrator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(calibrator
  main.cpp
  bone_calibrator_window.cpp
  bone_calibrator_window.h
  bone_calibrator_window.ui
  new_profile_dialog.cpp
  new_profile_dialog.h
  new_profile_dialog.ui
)

target_link_libraries(calibrator PRIVATE Qt5::Widgets)

if (DEFINED Qt5_DIR)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
find_file(QT_CORE_DLL
	NAMES
		Qt5Core.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

find_file(QT_GUI_DLL
	NAMES
		Qt5Gui.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

find_file(QT_WIDGET_DLL
	NAMES
		Qt5Widgets.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

else()

find_file(QT_CORE_DLL
	NAMES
		Qt5Cored.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

find_file(QT_GUI_DLL
	NAMES
		Qt5Guid.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

find_file(QT_WIDGET_DLL
	NAMES
		Qt5Widgetsd.dll
	PATHS
		${Qt5_DIR}/../../../bin/
	NO_DEFAULT_PATH
)

endif()

file(COPY ${QT_CORE_DLL}
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(COPY ${QT_GUI_DLL}
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(COPY ${QT_WIDGET_DLL}
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()


install(TARGETS calibrator RUNTIME DESTINATION k4a_openvr)

install(FILES ${QT_CORE_DLL} ${QT_GUI_DLL} ${QT_WIDGET_DLL} DESTINATION k4a_openvr)