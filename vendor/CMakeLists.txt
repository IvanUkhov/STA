set(HOTSPOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/HotSpot-${HOTSPOT_VER})

add_custom_target(hotspot
	COMMAND ${CMAKE_BUILD_TOOL} -C ${HOTSPOT_DIR}
)
add_custom_command(
	TARGET hotspot
	POST_BUILD
	COMMAND ln -fs ${HOTSPOT_DIR}/hotspot ${CMAKE_CURRENT_BINARY_DIR}/hotspot
	COMMAND ln -sfn ${HOTSPOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/hotspot
)
