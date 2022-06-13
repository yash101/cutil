cmake_minimum_required(VERSION 3.16)

# Project definition
project(
  cutil
  VERSION 1.0
  DESCRIPTION "Helper C library with basic constructs, data structures and algorithms"
  LANGUAGES C
)

# build configuration options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOC "Build documentation" ON)
set(CMAKE_BUILD_TYPE Debug)

# Output directories for outputs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include directory
include_directories(include)

# build documentation
add_subdirectory(docs)

# build library
add_subdirectory(src)

if (BUILD_TESTS)
  enable_language(CXX)
  set(CXX_STANDARD_REQUIRED 11)
  # build tests
  include(CTest)
  add_subdirectory(test)
endif()
