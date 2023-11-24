cmake_minimum_required(VERSION 3.24...3.28)

project(
  ugly_getopt
  VERSION 0.1.2
  LANGUAGES CXX)

include(GNUInstallDirs)

option(UGLY_ENABLE_TESTING "Enable tests" OFF)
option(UGLY_BUILD_EXAMPLES "Build examples" OFF)
option(UGLY_INSTALL "Enable installation of ugly_getopt. (Projects embedding ugly_getopt may want to turn this OFF.)" ON)

# ---- Declare library ----

add_library(ugly__getopt INTERFACE)
add_library(ugly::getopt ALIAS ugly__getopt)

target_link_libraries(ugly__getopt INTERFACE $<BUILD_INTERFACE:fmt::fmt>)
# target_link_libraries(ugly__getopt INTERFACE fmt::fmt)

set_property(
    TARGET ugly__getopt PROPERTY
    EXPORT_NAME ugly_getopt
)

set_target_properties(
  ugly__getopt
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(
  ugly__getopt
  INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

# ---- Install rules ----

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(package ugly_getopt)

# Allow package maintainers to freely override the path for the configs
set(
    ugly_getopt_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(promille_INSTALL_CMAKEDIR)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/Config.cmake.in
  ${PROJECT_BINARY_DIR}/ugly_getoptConfig.cmake
  INSTALL_DESTINATION ${ugly_getopt_INSTALL_CMAKEDIR})

write_basic_package_version_file(ugly_getoptConfigVersion.cmake
                             COMPATIBILITY SameMajorVersion
                             ARCH_INDEPENDENT)

if (UGLY_INSTALL)
  install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT ugly_getopt_Development
  )

  install(
    TARGETS ugly__getopt
    EXPORT ugly_getopt_Targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(
    FILES "${PROJECT_BINARY_DIR}/ugly_getoptConfig.cmake"
    DESTINATION "${ugly_getopt_INSTALL_CMAKEDIR}"
    COMPONENT ugly_getopt_Development
  )

  install(
    EXPORT ugly_getopt_Targets
    NAMESPACE ugly::
    DESTINATION "${ugly_getopt_INSTALL_CMAKEDIR}"
    FILE "ugly_getopt-targets.cmake"
    COMPONENT ugly_getopt_Development)

  # generate the export targets for the build tree needs to be after the
  # install(TARGETS ) command
  # export(EXPORT ugly_getopt_Targets
         # FILE "${PROJECT_BINARY_DIR}/ugly_getopt-targets.cmake")
endif()

# Export the package for use from the build-tree (this registers the build-tree
# with a global CMake-registry)
# export(PACKAGE ugly_getopt)

# ---- Examples ----

if(UGLY_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

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
