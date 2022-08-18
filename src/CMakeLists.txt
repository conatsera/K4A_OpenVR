add_library(k4a_openvr STATIC
    watchdog.cc
    driver.cc
    provider.cc
    driverlog.cc
)

target_include_directories(k4a_openvr
    PUBLIC
        "${OPENVR_INCLUDE_DIR}"
        "${K4A_INCLUDE_DIRS}"
)

target_link_libraries(k4a_openvr
    PUBLIC
        "${OPENVR_LIBRARIES}"
        "${K4A_LIBS}"
        ${GRPC_LIBS}
        calibration_grpc_proto
)

add_library(driver_k4a_openvr SHARED dllmain.cc)

target_link_libraries(driver_k4a_openvr k4a_openvr)

install(TARGETS driver_k4a_openvr DESTINATION k4a_openvr/bin/win64)