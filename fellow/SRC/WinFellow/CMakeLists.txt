# FellowNG Linux/SDL migration target for the legacy Fellow emulation core.
#
# Grow this target incrementally as legacy modules are made host-neutral.  It
# deliberately compiles real WinFellow emulation sources without linking the
# Windows frontend.

add_library(FellowNG.Emulation.Portable OBJECT
  C/CopperRegisters.cpp
  C/LegacyCopper.cpp
  C/LineExactCopper.cpp
  C/chipset.cpp
  graphics/CycleExactCopper.cpp
  graphics/DDFStateMachine.cpp
  graphics/DIWXStateMachine.cpp
  graphics/DIWYStateMachine.cpp
)

target_include_directories(FellowNG.Emulation.Portable
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core
)

target_compile_features(FellowNG.Emulation.Portable PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options(FellowNG.Emulation.Portable PRIVATE /W4)
else()
  target_compile_options(FellowNG.Emulation.Portable PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(FellowNG.Emulation.Portable PROPERTIES
  CXX_EXTENSIONS OFF
)
