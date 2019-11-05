add_library(driver_k4a_openvr SHARED dllmain.cpp)

target_include_directories(driver_k4a_openvr PUBLIC
	"${K4A_SDK_INCLUDE}"
	"${K4ABT_SDK_INCLUDE}"
)

target_link_libraries(driver_k4a_openvr k4a_driver)