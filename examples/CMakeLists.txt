# ------------------------------------------------------------------------------
# cmake - scripts
check_3rd_party_tool(
  cmake-scripts 9999 https://github.com/StableCoder/cmake-scripts TAG
  774a07ece3945b4f72e3ddc11646ab7cc9a349e2 AUTO)

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
