INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ITPP itpp)

if(PC_ITPP_FOUND)
  # look for include files
  FIND_PATH(
    ITPP_INCLUDE_DIRS
    NAMES itpp/itbase.h
    HINTS $ENV{ITPP_DIR}/include
          ${PC_ITPP_INCLUDE_DIRS}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
    )

  # look for libs
  FIND_LIBRARY(
    ITPP_LIBRARIES
    NAMES itpp 
    HINTS $ENV{ITPP_DIR}/lib
          ${PC_ITPP_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib/
          ${CMAKE_INSTALL_PREFIX}/lib64/
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
    )

  set(ITPP_FOUND ${PC_ITPP_FOUND})
endif(PC_ITPP_FOUND)

INCLUDE(FindPackageHandleStandardArgs)
# do not check ITPP_INCLUDE_DIRS, is not set when default include path us used.
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ITPP DEFAULT_MSG ITPP_LIBRARIES)
MARK_AS_ADVANCED(ITPP_LIBRARIES ITPP_INCLUDE_DIRS)
