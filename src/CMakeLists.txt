set(R2D2SRC ${R2D2SRC} r2d2.cpp usb.cpp motors.cpp sensors.cpp comm.cpp)

SET_SOURCE_FILES_PROPERTIES(${R2D2SRC} PROPERTIES COMPILE_FLAGS "${CXX11_COMPILER_FLAGS}")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(LIBBLUETOOTH bluetooth)
    set(R2D2LIBS ${R2D2LIBS} ${LIBBLUETOOTH})

    SET_SOURCE_FILES_PROPERTIES(bluetooth.cpp PROPERTIES COMPILE_FLAGS "${CXX11_COMPILER_FLAGS}")
    set(R2D2SRC ${R2D2SRC} bluetooth.cpp bluetooth_bridge.c)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET_SOURCE_FILES_PROPERTIES(osx_bluetooth.cpp PROPERTIES COMPILE_FLAGS "${CXX11_COMPILER_FLAGS}")
    set(R2D2SRC ${R2D2SRC} osx_bluetooth.cpp osx_bluetooth_bridge.m)
    set(CMAKE_SHARED_LINKER_FLAGS "-framework Foundation -framework IOBluetooth")
endif()

add_library(r2d2shared SHARED ${R2D2SRC})
add_library(r2d2static STATIC ${R2D2SRC})

target_link_libraries(r2d2shared ${R2D2LIBS})

set_target_properties(r2d2static PROPERTIES OUTPUT_NAME r2d2)
set_target_properties(r2d2shared PROPERTIES OUTPUT_NAME r2d2)
