set(TUNING_FLAGS "")

set(COMMON_SRCS
	matrix.cpp
	configuration.cpp
)

set(ANALYZE_SRCS
	analyze.cpp
	EigenvalueDecomposition.cpp
	HotSpot.cpp
	TemperatureAnalysis.cpp
	Variation.cpp
)

add_executable(analyze ${ANALYZE_SRCS} ${COMMON_SRCS})
target_link_libraries(analyze libhotspot)

set(DEPLOY_SRCS
	deploy.cpp
	Processor.cpp
	Platform.cpp
	Task.cpp
	Graph.cpp
	Priority.cpp
	GraphAnalysis.cpp
	Schedule.cpp
	DynamicPower.cpp
)

add_executable(deploy ${DEPLOY_SRCS} ${COMMON_SRCS})
target_link_libraries(deploy)

set(DESIGN_SRCS
	design.cpp
)

add_executable(design ${DESIGN_SRCS})
target_link_libraries(design)

include_directories(
	${VENDOR_DIR}
)

set(CMAKE_CXX_FLAGS
	"${TUNING_FLAGS} -Wall -Wno-deprecated-declarations -Wno-sign-compare -O3 $ENV{CFLAGS}"
)

add_library(libhotspot STATIC IMPORTED)
set_property(
	TARGET libhotspot PROPERTY
	IMPORTED_LOCATION ${VENDOR_DIR}/hotspot/libhotspot.a
)
