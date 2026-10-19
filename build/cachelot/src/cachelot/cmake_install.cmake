# Install script for directory: /home/itsc/b/cachelot/src/cachelot

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebugInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/itsc/b/cachelot/lib/RelWithDebugInfo/libcachelot.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/itsc/b/build/cachelot/src/cachelot/CMakeFiles/cachelot.dir/install-cxx-module-bmi-RelWithDebugInfo.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/itsc/b/cachelot/src/cachelot/bits.h"
    "/home/itsc/b/cachelot/src/cachelot/slice.h"
    "/home/itsc/b/cachelot/src/cachelot/cache.h"
    "/home/itsc/b/cachelot/src/cachelot/common.h"
    "/home/itsc/b/cachelot/src/cachelot/dict.h"
    "/home/itsc/b/cachelot/src/cachelot/error.h"
    "/home/itsc/b/cachelot/src/cachelot/expiration_clock.h"
    "/home/itsc/b/cachelot/src/cachelot/hash_fnv1a.h"
    "/home/itsc/b/cachelot/src/cachelot/hash_table.h"
    "/home/itsc/b/cachelot/src/cachelot/intrusive_list.h"
    "/home/itsc/b/cachelot/src/cachelot/item.h"
    "/home/itsc/b/cachelot/src/cachelot/memalloc-inl.h"
    "/home/itsc/b/cachelot/src/cachelot/memalloc.h"
    "/home/itsc/b/cachelot/src/cachelot/random.h"
    "/home/itsc/b/cachelot/src/cachelot/stats.h"
    "/home/itsc/b/cachelot/src/cachelot/string_conv.h"
    "/home/itsc/b/cachelot/src/cachelot/version.h"
    "/home/itsc/b/cachelot/src/cachelot/c_api.h"
    )
endif()

