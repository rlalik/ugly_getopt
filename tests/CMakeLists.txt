find_package(GTest)

if(NOT GTest_FOUND)
  message(STATUS "Add GoogleTest from github")
  check_3rd_party_tool(googletest 9999 https://github.com/google/googletest TAG
                       v1.13.0 AUTO)
endif()

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
