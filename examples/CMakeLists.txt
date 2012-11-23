link_directories (${R2D2_BINARY_DIR}/src) 

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wall HAS_WALL)

if (HAS_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

check_cxx_compiler_flag(--std=c++0x HAS_STD_CPP0X)
if (HAS_STD_CPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
endif()

add_executable(test test.cpp)
add_executable(mary mary.cpp)
target_link_libraries(test r2d2shared)
target_link_libraries(mary r2d2shared)
