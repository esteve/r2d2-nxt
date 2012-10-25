cmake_minimum_required (VERSION 2.6)
project (R2D2-NXT)

add_definitions(-Wall -std=c++0x)
include_directories(include)
add_subdirectory(src)

#set_target_properties(r2d2 PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
