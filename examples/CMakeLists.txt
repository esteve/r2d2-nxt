link_directories (${R2D2_BINARY_DIR}/src) 

add_executable(test test.cpp)
add_executable(mary mary.cpp)
target_link_libraries(test r2d2shared)
target_link_libraries(mary r2d2shared)
