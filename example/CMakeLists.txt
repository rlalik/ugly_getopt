cmake_minimum_required(VERSION 3.24)

project(ugly_getoptExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(ugly_getopt REQUIRED)
endif()

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

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE ugly::getopt)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(basic_usage)
add_example(fluent_usage)
add_example(full_usage)
add_example(example_readme)

add_folders(Example)
