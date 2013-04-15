link_directories (${R2D2_BINARY_DIR}/src) 

SET_SOURCE_FILES_PROPERTIES(test.cpp test_bt.cpp mary.cpp mary_bt.cpp avoid_obstacles_bt.cpp PROPERTIES COMPILE_FLAGS "${CXX11_COMPILER_FLAGS}")

add_executable(test test.cpp)
add_executable(test_bt test_bt.cpp)
add_executable(mary mary.cpp)
add_executable(mary_bt mary_bt.cpp)
add_executable(avoid_obstacles_bt avoid_obstacles_bt.cpp)
target_link_libraries(test r2d2shared)
target_link_libraries(test_bt r2d2shared)
target_link_libraries(mary r2d2shared)
target_link_libraries(mary_bt r2d2shared)
target_link_libraries(avoid_obstacles_bt r2d2shared)
