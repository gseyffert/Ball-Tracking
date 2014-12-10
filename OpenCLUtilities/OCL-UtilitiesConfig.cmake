# Find the OCL-Utilities includes and library
#
# OCL-Utilities_INCLUDE_DIR - where to find include files.
# OCL-Utilities_LIBRARIES - List of fully qualified libraries to link against.
# OCL-Utilities_FOUND - set to 1 if found.
#
# Usage:
# OCL-Utilities (OCL-Utilities)
#----------
# option(CX_USE_OCL-Utilities "use OCL-Utilities (OCL-Utilities)" OFF)
# if (CX_USE_OCL-Utilities)
#    ADD_DEFINITIONS(-DCX_USE_OCL-Utilities)
#    find_package(OCL-Utilities REQUIRED)
#    include()
# endif()
#

set (OCL-Utilities_FOUND 1)

set (OCL-Utilities_SOURCE_DIR "/Users/maxpollard/desktop/repos/Ball-Tracking/OpenCLUtilities")

set (OCL-Utilities_INCLUDE_DIRS "/Users/maxpollard/desktop/repos/Ball-Tracking/OpenCLUtilities")
set (OCL-Utilities_LIBRARY_DIRS "/Users/maxpollard/desktop/repos/Ball-Tracking/OpenCLUtilities")

set (OCL-Utilities_USE_FILE "/Users/maxpollard/desktop/repos/Ball-Tracking/OpenCLUtilities/CMake/OCL-UtilitiesUse.cmake")

