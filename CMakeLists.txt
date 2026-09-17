cmake_minimum_required(VERSION 3.20)

project(FellowNG VERSION 0.1.0 LANGUAGES C CXX)

option(FELLOWNG_BUILD_TESTS "Build FellowNG portable-core tests" ON)
option(FELLOWNG_BUILD_SDL_FRONTEND "Build the FellowNG SDL3 frontend" OFF)
option(FELLOWNG_BUILD_EMULATION_CORE "Compile migrated WinFellow emulation-core slices" ON)

add_subdirectory(fellow/SRC/WinFellow.Core)

if(FELLOWNG_BUILD_EMULATION_CORE)
  add_subdirectory(fellow/SRC/WinFellow)
endif()

if(FELLOWNG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/portable-core)
endif()

if(FELLOWNG_BUILD_SDL_FRONTEND)
  add_subdirectory(frontend/sdl)
endif()
