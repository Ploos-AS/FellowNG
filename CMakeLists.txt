cmake_minimum_required(VERSION 3.20)

project(FellowNG VERSION 0.1.0 LANGUAGES CXX)

option(FELLOWNG_BUILD_TESTS "Build FellowNG portable-core tests" ON)

add_subdirectory(fellow/SRC/WinFellow.Core)

if(FELLOWNG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/portable-core)
endif()
