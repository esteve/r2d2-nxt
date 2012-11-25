cmake_minimum_required (VERSION 2.8)
project (R2D2-NXT)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
endif()

# Set library path to ports path on Mac OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(LIBBLUETOOTH bluetooth)
    set(R2D2LIBS ${R2D2LIBS} ${LIBBLUETOOTH})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_package(Boost COMPONENTS thread system REQUIRED)
else()
    find_package(Boost COMPONENTS thread REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS})
set(R2D2LIBS ${R2D2LIBS} ${Boost_LIBRARIES})

find_library(LIBUSB usb-1.0)
set(R2D2LIBS ${R2D2LIBS} ${LIBUSB})

include_directories(include)
add_subdirectory(src)
add_subdirectory(examples)
