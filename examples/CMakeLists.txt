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

find_or_fetch_package(cmake-scripts https://github.com/StableCoder/cmake-scripts GIT_TAG main AUTO)
include(GoogleTest)
list(APPEND CMAKE_MODULE_PATH ${cmake-scripts_SOURCE_DIR})

# include(c++-standards)
include(code-coverage)
include(sanitizers)
# include(doxygen)
# include(formatting)
# include(tools)
# ==============================================================================

add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage ugly_getopt::ugly_getopt)

add_executable(fluent_usage fluent_usage.cpp)
target_link_libraries(fluent_usage ugly_getopt::ugly_getopt)
