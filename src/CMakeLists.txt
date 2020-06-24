# CMakeList.txt : CMake project for K4A OpenVR, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.14)

add_subdirectory("driver")

add_subdirectory("provider")

#add_subdirectory("calibrator")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_subdirectory("windows")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	#add_subdirectory("linux") TODO
else()
    message(FATAL_ERROR "Unknown system: ${CMAKE_SYSTEM_NAME}")
endif()
