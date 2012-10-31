ADD_LIBRARY(r2d2 SHARED r2d2.cpp)
ADD_LIBRARY(r2d2static STATIC r2d2.cpp)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-Wall HAS_WALL)
if (HAS_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

check_cxx_compiler_flag(--std=c++0x HAS_STD_C0X)
if (HAS_STD_C0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
endif()

set_target_properties(r2d2static PROPERTIES
         OUTPUT_NAME r2d2)
