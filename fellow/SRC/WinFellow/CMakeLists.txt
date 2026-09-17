# FellowNG Linux/SDL migration target for the legacy Fellow emulation core.
#
# Grow this target incrementally as legacy modules are made host-neutral.  It
# deliberately compiles real WinFellow emulation sources without linking the
# Windows frontend.

add_library(FellowNG.Emulation.Portable OBJECT
  C/Blitter.cpp
  C/BusScheduler.cpp
  C/ComplexInterfaceAdapter.cpp
  C/CopperRegisters.cpp
  C/CpuIntegration.c
  C/CpuModule.c
  C/CpuModule_EffectiveAddress.c
  C/CpuModule_Exceptions.c
  C/CpuModule_Flags.c
  C/CpuModule_Instructions.c
  C/CpuModule_InternalState.c
  C/CpuModule_Interrupts.c
  C/CpuModule_StackFrameGen.c
  C/Keyboard.cpp
  C/LegacyCopper.cpp
  C/LineExactCopper.cpp
  C/LineExactSprites.cpp
  C/MemoryInterface.cpp
  C/SpriteMerger.cpp
  C/SpriteP2CDecoder.cpp
  C/SpriteRegisters.cpp
  C/Sprites.cpp
  C/chipset.cpp
  C/rtc.cpp
  graphics/BitplaneDMA.cpp
  graphics/BitplaneDraw.cpp
  graphics/CycleExactCopper.cpp
  graphics/CycleExactSprites.cpp
  graphics/DDFStateMachine.cpp
  graphics/DIWXStateMachine.cpp
  graphics/DIWYStateMachine.cpp
  graphics/Graphics.cpp
  graphics/GraphicsEvent.cpp
  graphics/GraphicsEventQueue.cpp
  graphics/Logger.cpp
  graphics/PixelSerializer.cpp
  graphics/Planar2ChunkyDecoder.cpp
)

target_include_directories(FellowNG.Emulation.Portable
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core
)

target_compile_features(FellowNG.Emulation.Portable PRIVATE c_std_99 cxx_std_20)

if(MSVC)
  target_compile_options(FellowNG.Emulation.Portable PRIVATE /W4)
else()
  target_compile_options(FellowNG.Emulation.Portable PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(FellowNG.Emulation.Portable PROPERTIES
  C_EXTENSIONS OFF
  CXX_EXTENSIONS OFF
)
