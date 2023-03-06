cmake_minimum_required(VERSION 3.16)
project(
  ugly_getopt
  VERSION 0.0.5
  LANGUAGES C CXX)

option(UGLY_ENABLE_TESTING "Enable tests" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

# ==============================================================================
# fetch external content
include(FetchContent)
# ------------------------------------------------------------------------------
# cmake_tools
FetchContent_Declare(
  cmake_tools
  GIT_REPOSITORY https://github.com/rlalik/cmake_tools
  GIT_TAG master)

FetchContent_MakeAvailable(cmake_tools)
list(APPEND CMAKE_MODULE_PATH ${cmake_tools_SOURCE_DIR})
include(check_3rd_party_tool)

# ==============================================================================
# build project

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
add_library(ugly_getopt INTERFACE)
add_library(ugly_getopt::ugly_getopt ALIAS ugly_getopt)

set_target_properties(
  ugly_getopt
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             PUBLIC_HEADER "ugly_getopt.h")

target_include_directories(
  ugly_getopt INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

# installing

if(NOT DEFINED ugly_getopt_INSTALL_CMAKEDIR)
  set(ugly_getopt_INSTALL_CMAKEDIR
      "${CMAKE_INSTALL_LIBDIR}/cmake/ugly_getopt"
      CACHE STRING "Path to ugly_getopt CMake files")
endif()

install(
  TARGETS ugly_getopt
  EXPORT ugly_getopt_Targets
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development)

# Export the package for use from the build-tree (this registers the build-tree
# with a global CMake-registry)
export(PACKAGE ugly_getopt)

install(
  EXPORT ugly_getopt_Targets
  DESTINATION "${ugly_getopt_INSTALL_CMAKEDIR}"
  NAMESPACE ugly_getopt::
  FILE "ugly_getopt-targets.cmake"
  COMPONENT Development)

# generate the export targets for the build tree needs to be after the
# install(TARGETS ) command
export(EXPORT ugly_getopt_Targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/ugly_getopt-targets.cmake")

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ugly_getoptConfig.cmake
  INSTALL_DESTINATION ${ugly_getopt_INSTALL_CMAKEDIR})

write_basic_package_version_file(ugly_getoptConfigVersion.cmake
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ugly_getoptConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ugly_getoptConfigVersion.cmake
        DESTINATION ${ugly_getopt_INSTALL_CMAKEDIR})

add_subdirectory(examples)

# tests
if(UGLY_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# messages
message(
  STATUS
    "<<< Configuration >>>
Project:        ugly_getopt

Architecture:   ${CMAKE_LIBRARY_ARCHITECTURE}
Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}

Compiler:
C               ${CMAKE_C_COMPILER}
C++             ${CMAKE_CXX_COMPILER}

Linker:
Ld              ${CMAKE_LINKER}

Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}

Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}")

foreach(p LIB BIN INCLUDE CMAKE)
  message(STATUS "CMAKE_INSTALL_${p}DIR: ${CMAKE_INSTALL_${p}DIR}")
endforeach()
