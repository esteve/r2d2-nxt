find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h
          PATH_SUFFIXES libusb-1.0
          DOC "The LIBUSB (libusb-1.0) include directory"
)

find_library(LIBUSB_LIBRARY NAMES usb-1.0
          DOC "The LIBUSB (libusb-1.0) library"
)

# handle the QUIETLY and REQUIRED arguments and set LIBUSB_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUSB DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)

if(LIBUSB_FOUND)
  set( LIBUSB_LIBRARIES ${LIBUSB_LIBRARY} )
  set( LIBUSB_INCLUDE_DIRS ${LIBUSB_INCLUDE_DIR} )
endif()

mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
