cmake_minimum_required (VERSION 2.8)
project (R2D2-NXT)

# Add project's CMake modules
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

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

# Check that the C++ compiler supports C++11, or at least C++0x
check_cxx_compiler_flag(--std=c++0x HAS_STD_CPP0X)
if (HAS_STD_CPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
endif()

check_cxx_compiler_flag(-std=c++11 HAS_STD_CPP11)
if (HAS_STD_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(NOT(HAS_STD_CPP11 OR HAS_STD_CPP0X))
    message("${PROJECT_NAME} requires a compiler that supports C++11")
    return()
endif()

# Set library path to ports path on Mac OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)

    # Use Clang's libc++ on Mac OSX
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Mac OSX requires boost.system
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_package(Boost COMPONENTS thread system REQUIRED)
else()
    find_package(Boost COMPONENTS thread REQUIRED)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Work around Boost bug #7671 (https://svn.boost.org/trac/boost/ticket/7671)
    if(${Boost_VERSION} STREQUAL "105200")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_NO_CXX11_NUMERIC_LIMITS")
    endif()
endif()

# Require Boost
include_directories(${Boost_INCLUDE_DIRS})
set(R2D2LIBS ${R2D2LIBS} ${Boost_LIBRARIES})

# Require libusb-1.0
find_package(LIBUSB REQUIRED)
include_directories(${LIBUSB_INCLUDE_DIRS})
set(R2D2LIBS ${R2D2LIBS} ${LIBUSB_LIBRARIES})

# Add the Bluetooth libraries on Linux only
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(BLUETOOTH REQUIRED)
    include_directories(${LIBBLUETOOTH_INCLUDE_DIRS})
    set(R2D2LIBS ${R2D2LIBS} ${LIBBLUETOOTH_LIBRARIES})
endif()

include_directories(include)
add_subdirectory(src)

# TODO: make the examples optional
add_subdirectory(examples)
