# - Try to find the libusb-1.0 USB library
# Once done this will define
#
#  LIBUSB1_FOUND - System has LibUsb1
#  LIBUSB1_INCLUDE_DIR - The LibUsb1 include directory
#  LIBUSB1_LIBRARIES - The libraries needed to use LibUsb1
#  LIBUSB1_DEFINITIONS - Compiler switches required for using LibUsb1

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_LIBUSB1 libusb-1.0 QUIET)
SET(LIBUSB1_DEFINITIONS ${PC_LIBUSB1_CFLAGS_OTHER})

FIND_PATH(LIBUSB1_INCLUDE_DIR NAMES libusb-1.0/libusb.h
   HINTS
   ${PC_LIBUSB1_INCLUDEDIR}
   ${PC_LIBUSB1_INCLUDE_DIRS}
   PATH_SUFFIXES libusb-1.0
   )

FIND_LIBRARY(LIBUSB1_LIBRARIES NAMES usb-1.0
   HINTS
   ${PC_LIBUSB1_LIBDIR}
   ${PC_LIBUSB1_LIBRARY_DIRS}
   )

# handle the QUIETLY and REQUIRED arguments and set LIBUSB1_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibUsb1 DEFAULT_MSG LIBUSB1_LIBRARIES LIBUSB1_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBUSB1_INCLUDE_DIR LIBUSB1_LIBRARIES)
