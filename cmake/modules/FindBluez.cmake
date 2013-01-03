# - Try to find the Bluez bluetooth stack for Linux
# Once done this will define
#
#  BLUEZ_FOUND - System has Bluez
#  BLUEZ_INCLUDE_DIR - The Bluez include directory
#  BLUEZ_LIBRARIES - The libraries needed to use Bluez
#  BLUEZ_DEFINITIONS - Compiler switches required for using Bluez

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_BLUEZ bluez QUIET)
SET(BLUEZ_DEFINITIONS ${PC_BLUEZ_CFLAGS_OTHER})

FIND_PATH(BLUEZ_INCLUDE_DIR NAMES bluetooth/bluetooth.h
   HINTS
   ${PC_BLUEZ_INCLUDEDIR}
   ${PC_BLUEZ_INCLUDE_DIRS}
   PATH_SUFFIXES bluetooth
   )

FIND_LIBRARY(BLUEZ_LIBRARIES NAMES bluetooth
   HINTS
   ${PC_BLUEZ_LIBDIR}
   ${PC_BLUEZ_LIBRARY_DIRS}
   )

# handle the QUIETLY and REQUIRED arguments and set BLUEZ_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bluez DEFAULT_MSG BLUEZ_LIBRARIES BLUEZ_INCLUDE_DIR)

MARK_AS_ADVANCED(BLUEZ_INCLUDE_DIR BLUEZ_LIBRARIES)
