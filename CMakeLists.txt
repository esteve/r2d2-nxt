cmake_minimum_required (VERSION 2.8)
project (R2D2-NXT)

# Add project's CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(CheckCXX11Features)
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

if(NOT(HAS_CXX11_CSTDINT_H AND HAS_CXX11_NULLPTR AND HAS_CXX11_ENUM_CLASS AND HAS_CXX11_MUTEX))
    message("${PROJECT_NAME} requires a compiler that supports the following C++11 features:")
    message("cstdint header")
    message("nullptr")
    message("enum class")
    message("mutex")
    return()
endif()

# Set library path to ports path on Mac OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)

    # Use Clang's libc++ on Mac OSX
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


SET_SOURCE_FILES_PROPERTIES(
    "*.cpp"
    PROPERTIES
        COMPILE_FLAGS ${CXX11_COMPILER_FLAGS})

# Require libusb-1.0
find_package(LibUsb1)
if(NOT LIBUSB1_FOUND)
    message(FATAL_ERROR "Required library libusb-1.0 NOT FOUND.\nInstall the library (dev version) and try again.")
endif()
include_directories(${LIBUSB1_INCLUDE_DIRS})
set(R2D2LIBS ${R2D2LIBS} ${LIBUSB1_LIBRARIES})

# Add the Bluetooth libraries on Linux only
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(Bluez)
    if(NOT LIBUSB1_FOUND)
        message(FATAL_ERROR "Required library Bluez (libbluetooth) NOT FOUND.\nInstall the library (dev version) and try again.")
    endif()
    include_directories(${BLUEZ_INCLUDE_DIRS})
    set(R2D2LIBS ${R2D2LIBS} ${BLUEZ_LIBRARIES})
endif()

include_directories(include)
add_subdirectory(src)

# TODO: make the examples optional
add_subdirectory(examples)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
          FILES_MATCHING PATTERN "*.hpp")
