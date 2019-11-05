add_library (k4a_driver STATIC
	"k4a_driver.cpp"
	"k4a_driver.h"
	"log.cpp"
	"log.h"
)

target_include_directories(k4a_driver
	PUBLIC
		"${OPENVR_INCLUDE_DIR}"
		"${K4A_INCLUDE_DIRS}"
)

target_link_libraries(k4a_driver
	PRIVATE
		k4a_driver_provider
	PUBLIC
		${OPENVR_LIBRARIES}
)

file(COPY ${OPENVR_DLL}
	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
