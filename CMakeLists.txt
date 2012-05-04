cmake_minimum_required (VERSION 2.6)

project(sta)

set(HOTSPOT_VER 5.02)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)

add_subdirectory(src)
add_subdirectory(vendor)
