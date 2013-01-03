link_directories (${R2D2_BINARY_DIR}/src) 

SET_SOURCE_FILES_PROPERTIES(test.cpp mary.cpp PROPERTIES COMPILE_FLAGS "${CXX11_COMPILER_FLAGS}")

add_executable(test test.cpp)
add_executable(mary mary.cpp)
target_link_libraries(test r2d2shared)
target_link_libraries(mary r2d2shared)
