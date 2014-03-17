INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_OP25 op25)

FIND_PATH(
    OP25_INCLUDE_DIRS
    NAMES op25/api.h
    HINTS $ENV{OP25_DIR}/include
        ${PC_OP25_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    OP25_LIBRARIES
    NAMES gnuradio-op25
    HINTS $ENV{OP25_DIR}/lib
        ${PC_OP25_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OP25 DEFAULT_MSG OP25_LIBRARIES OP25_INCLUDE_DIRS)
MARK_AS_ADVANCED(OP25_LIBRARIES OP25_INCLUDE_DIRS)

