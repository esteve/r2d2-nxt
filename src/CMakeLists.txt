set(R2D2SRC ${R2D2SRC} r2d2.cpp usb.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    find_library(LIBBLUETOOTH bluetooth)
    set(R2D2LIBS ${R2D2LIBS} ${LIBBLUETOOTH})

    set(R2D2SRC ${R2D2SRC} bluetooth.cpp bluetooth_bridge.c)
endif()

add_library(r2d2shared SHARED ${R2D2SRC})
add_library(r2d2static STATIC ${R2D2SRC})

target_link_libraries(r2d2shared ${R2D2LIBS})

set_target_properties(r2d2static PROPERTIES OUTPUT_NAME r2d2)
set_target_properties(r2d2shared PROPERTIES OUTPUT_NAME r2d2)
