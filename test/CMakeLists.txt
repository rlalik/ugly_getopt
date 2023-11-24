cmake_minimum_required(VERSION 3.24)

project(ugly_getoptTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(ugly_getopt REQUIRED)
  enable_testing()
endif()

include(FetchContent)

set(INSTALL_GTEST OFF)
FetchContent_Declare(
  GTest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.14.0
  FIND_PACKAGE_ARGS 1.14.0 NAMES GTest
)
FetchContent_MakeAvailable(GTest)

set(FMT_INSTALL OFF)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 10.1.1
  FIND_PACKAGE_ARGS 10.1.1
)
FetchContent_MakeAvailable(fmt)

include(GoogleTest)

set(tests_SRCS tests_parser_option.cpp tests_input_params.cpp tests_fluent.cpp)

add_executable(gtests ${tests_SRCS})

if(TARGET GTest::gtest_main)
  # new cmake provided
  set(GTEST_TRG GTest::gtest_main)
else()
  # old cmake and pc files
  set(GTEST_TRG gtest gtest_main)
endif()

target_link_libraries(gtests PRIVATE ${GTEST_TRG} ugly::getopt fmt::fmt)
target_compile_options(gtests PUBLIC -Wno-write-strings)

gtest_discover_tests(gtests)

# ---- End-of-file commands ----

add_folders(Test)
