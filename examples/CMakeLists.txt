# ==============================================================================
# fetch external content
include(FetchContent)

FetchContent_Declare(
  cmake_tools
  GIT_REPOSITORY https://github.com/rlalik/cmake_tools
  GIT_TAG master)

# FetchContent_MakeAvailable requires CMake-3.14 or newer
if(CMAKE_VERSION VERSION_LESS 3.14)
  FetchContent_GetProperties(cmake_tools)
  if(NOT cmake_tools_POPULATED)
    FetchContent_Populate(cmake_tools)
  endif()
else()
  FetchContent_MakeAvailable(cmake_tools)
endif()
list(APPEND CMAKE_MODULE_PATH ${cmake_tools_SOURCE_DIR})
include(find_or_fetch_package)

set(FMT_INSTALL OFF)
# cmake-format: off
find_or_fetch_package(fmt https://github.com/fmtlib/fmt
  GIT_TAG master AUTO)
# cmake-format: on

add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage ugly::getopt fmt::fmt)

add_executable(fluent_usage fluent_usage.cpp)
target_link_libraries(fluent_usage ugly::getopt fmt::fmt)

add_executable(full_usage full_usage.cpp)
target_link_libraries(full_usage ugly::getopt fmt::fmt)

add_executable(example_readme example_readme.cpp)
target_link_libraries(example_readme ugly::getopt fmt::fmt)
