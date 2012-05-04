set(TUNING_FLAGS "-DSHALLOW_CHECK")

set(COMMON_SRCS
	matrix.cpp
	configuration.cpp
	EigenvalueDecomposition.cpp
	HotSpot.cpp
)

add_executable(analyze analyze.cpp ${COMMON_SRCS})
target_link_libraries(analyze libhotspot)

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
