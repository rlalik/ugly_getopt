find_package(GTest)

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
include(find_or_fetch_package)

find_or_fetch_package(GTest https://github.com/google/googletest GIT_TAG main AUTO)
include(GoogleTest)

# configure_file(tests_config.h.in tests_config.h)

set(tests_SRCS tests_functions.cpp tests_input_params.cpp tests_fluent.cpp)

add_executable(gtests ${tests_SRCS})

if(TARGET GTest::gtest_main)
  # new cmake provided
  set(GTEST_TRG GTest::gtest_main)
else()
  # old cmake and pc files
  set(GTEST_TRG gtest gtest_main)
endif()

target_link_libraries(gtests ${GTEST_TRG} ugly_getopt::ugly_getopt)
target_code_coverage(gtests ALL)

gtest_discover_tests(gtests)
