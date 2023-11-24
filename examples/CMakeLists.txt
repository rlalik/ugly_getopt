# ==============================================================================
# fetch external content
set(FMT_INSTALL OFF)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 10.1.1
  FIND_PACKAGE_ARGS 10.1.1
)
FetchContent_MakeAvailable(fmt)

add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage ugly::getopt fmt::fmt)

add_executable(fluent_usage fluent_usage.cpp)
target_link_libraries(fluent_usage ugly::getopt fmt::fmt)

add_executable(full_usage full_usage.cpp)
target_link_libraries(full_usage ugly::getopt fmt::fmt)

add_executable(example_readme example_readme.cpp)
target_link_libraries(example_readme ugly::getopt fmt::fmt)
