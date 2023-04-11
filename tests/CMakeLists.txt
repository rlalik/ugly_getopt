option(ENABLE_ADVANCE_TOOLS "Enables advanced tools" OFF)

if(ENABLE_ADVANCE_TOOLS)
  FetchContent_Declare(
    cmake-scripts
    GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts
    GIT_TAG main)
  FetchContent_GetProperties(cmake-scripts)
  if(NOT cmake-scripts_POPULATED)
    FetchContent_Populate(cmake-scripts)
    list(APPEND CMAKE_MODULE_PATH ${cmake-scripts_SOURCE_DIR})
    message(STATUS "${cmake-scripts_SOURCE_DIR}")
  endif()

  list(APPEND CMAKE_MODULE_PATH ${cmake-scripts_SOURCE_DIR})

  include(sanitizers)

  include(code-coverage)
  add_code_coverage()

  include(tools)
  include_what_you_use(-Xiwyu;--error_always)
endif()

set(FMT_INSTALL OFF)
# cmake-format: off
find_or_fetch_package(GTest https://github.com/google/googletest
                      GIT_TAG main
                      AUTO)

find_or_fetch_package(fmt https://github.com/fmtlib/fmt
                      GIT_TAG master
                      AUTO)
# cmake-format: on
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

if(ENABLE_ADVANCE_TOOLS)
  target_code_coverage(gtests)
endif()

gtest_discover_tests(gtests)
