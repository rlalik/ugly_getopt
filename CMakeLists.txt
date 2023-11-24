cmake_minimum_required(VERSION 3.13...3.28)

include(cmake/prelude.cmake)

project(
  ugly_getopt
  VERSION 0.1.2
    DESCRIPTION "C++ wrapper around GNU getopt"
    HOMEPAGE_URL "https://github.com/rlalik/ugly_getopt/"
  LANGUAGES CXX)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

option(UGLY_INSTALL "Enable installation of ugly_getopt. (Projects embedding ugly_getopt may want to turn this OFF.)" ON)

# ---- Declare library ----

add_library(ugly_getopt_ugly_getopt INTERFACE)
add_library(ugly::getopt ALIAS ugly_getopt_ugly_getopt)

set_property(
    TARGET ugly_getopt_ugly_getopt PROPERTY
    EXPORT_NAME ugly_getopt
)

target_include_directories(
    ugly_getopt_ugly_getopt ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_link_libraries(ugly_getopt_ugly_getopt INTERFACE $<BUILD_INTERFACE:fmt::fmt>)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES AND UGLY_INSTALL)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${ugly_getopt_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT ugly_getopt_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of ugly_getopt"
  )
endif()

include(cmake/dev-mode.cmake)
