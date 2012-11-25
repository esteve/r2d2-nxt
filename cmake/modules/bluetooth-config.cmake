find_path(BLUETOOTH_INCLUDE_DIR NAMES bluetooth.h
          PATH_SUFFIXES bluetooth
          DOC "The BLUETOOTH (libbluetooth) include directory"
)

find_library(BLUETOOTH_LIBRARY NAMES bluetooth
          DOC "The BLUETOOTH (libbluetooth) library"
)

# handle the QUIETLY and REQUIRED arguments and set BLUETOOTH_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BLUETOOTH DEFAULT_MSG BLUETOOTH_LIBRARY BLUETOOTH_INCLUDE_DIR)

if(BLUETOOTH_FOUND)
  set( BLUETOOTH_LIBRARIES ${BLUETOOTH_LIBRARY} )
  set( BLUETOOTH_INCLUDE_DIRS ${BLUETOOTH_INCLUDE_DIR} )
endif()

mark_as_advanced(BLUETOOTH_INCLUDE_DIR BLUETOOTH_LIBRARY)
