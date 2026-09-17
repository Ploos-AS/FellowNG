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
  C/FellowList.cpp
  C/GraphicsPipeline.cpp
  C/Keyboard.cpp
  C/LegacyCopper.cpp
  C/LineExactCopper.cpp
  C/LineExactSprites.cpp
  C/MemoryInterface.cpp
  C/Renderer.cpp
  C/SpriteMerger.cpp
  C/SpriteP2CDecoder.cpp
  C/SpriteRegisters.cpp
  C/Sprites.cpp
  C/chipset.cpp
  C/draw_interlace_control.cpp
  C/draw_pixelrenderers.cpp
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

# These historical .c CPU units are part of the WinFellow C++ core in
# practice: they include shared headers such as Defs.h that use the C++
# standard library.  Keep their filenames for lineage, but compile them with
# the C++ frontend on portable hosts instead of treating them as ISO C.
set_source_files_properties(
  C/CpuIntegration.c
  C/CpuModule.c
  C/CpuModule_EffectiveAddress.c
  C/CpuModule_Exceptions.c
  C/CpuModule_Flags.c
  C/CpuModule_Instructions.c
  C/CpuModule_InternalState.c
  C/CpuModule_Interrupts.c
  C/CpuModule_StackFrameGen.c
  PROPERTIES LANGUAGE CXX
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

  # GraphicsPipeline still contains one legacy WinFellow callback assignment
  # that routes a function pointer through void*. GCC rejects that historical
  # MSVC-compatible conversion in strict C++ mode. Keep the exception scoped
  # to this translation unit while the renderer ABI is migrated, rather than
  # weakening the portable target globally.
  set_source_files_properties(C/GraphicsPipeline.cpp PROPERTIES
    COMPILE_OPTIONS "-fpermissive"
  )

  # CpuModule_Instructions contains MSVC x86 profiling assembly plus an
  # optional host calltrap hook. Keep those legacy host details out of the
  # Linux build without weakening or forking the 68k instruction core.
  set_source_files_properties(C/CpuModule_Instructions.c PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableCpuCompat.h"
  )

  set_source_files_properties(C/Renderer.cpp PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableRendererCompat.h"
  )
endif()

set_target_properties(FellowNG.Emulation.Portable PROPERTIES
  C_EXTENSIONS OFF
  CXX_EXTENSIONS OFF
)
