# Filename: FindOQS.cmake
# Authors: darktohka (27 Jul, 2022)
#
# Usage:
#   find_package(OQS [REQUIRED] [QUIET])
#
# Once done this will define:
#   OQS_FOUND       - system has liboqs
#   OQS_INCLUDE_DIR - the include directory containing oqs/
#   OQS_LIBRARY     - the path to the liboqs library
#

find_path(OQS_INCLUDE_DIR NAMES "oqs/common.h")

find_library(OQS_LIBRARY NAMES "oqs")

mark_as_advanced(OQS_INCLUDE_DIR OQS_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OQS DEFAULT_MSG OQS_INCLUDE_DIR OQS_LIBRARY)
