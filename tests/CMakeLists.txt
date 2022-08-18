add_executable(provider-ut
	provider-ut.cc
)

target_include_directories(provider-ut
	PUBLIC
		"${OPENVR_INCLUDE_DIR}"
		"${K4A_INCLUDE_DIRS}"
)

target_link_libraries(provider-ut
	PUBLIC
		"${OPENVR_LIBRARIES}"
		k4a_openvr
		${GRPC_LIBS}
		calibration_grpc_proto
)

add_executable(openvr_pose_avg-ut
	openvr_pose_avg-ut.cc
)

target_include_directories(openvr_pose_avg-ut
	PUBLIC
		"${OPENVR_INCLUDE_DIR}"
)

target_link_libraries(openvr_pose_avg-ut
	PUBLIC
		"${OPENVR_LIBRARIES}"
)
