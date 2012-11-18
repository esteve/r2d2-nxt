include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

check_c_compiler_flag(-Wall C_HAS_WALL)
if (C_HAS_WALL)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

check_cxx_compiler_flag(-Wall CXX_HAS_WALL)
if (CXX_HAS_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

check_cxx_compiler_flag(--std=c++0x HAS_STD_CPP0X)
if (HAS_STD_CPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
endif()

check_cxx_compiler_flag(-std=c++11 HAS_STD_CPP11)
if (HAS_STD_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(R2D2SRC ${R2D2SRC} r2d2.cpp usb.cpp)
set(R2D2LIBS ${R2D2LIBS} usb-1.0)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(R2D2LIBS ${R2D2LIBS} bluetooth)
    set(R2D2SRC ${R2D2SRC} bluetooth.cpp bluetooth_bridge.c)
endif()

find_package(Boost COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
set(R2D2LIBS ${R2D2LIBS} ${Boost_LIBRARIES})

add_library(r2d2shared SHARED ${R2D2SRC})
add_library(r2d2static STATIC ${R2D2SRC})

target_link_libraries(r2d2shared ${R2D2LIBS})

set_target_properties(r2d2static PROPERTIES OUTPUT_NAME r2d2)
set_target_properties(r2d2shared PROPERTIES OUTPUT_NAME r2d2)
