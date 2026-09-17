add_library(FellowNG.Emulation.Portable STATIC
  C/Blitter.cpp
  C/BusScheduler.cpp
  C/ComplexInterfaceAdapter.cpp
  C/Configuration.cpp
  C/CopperRegisters.cpp
  C/CpuIntegration.c
  C/CpuModule.c
  C/CpuModule_Disassembler.c
  C/CpuModule_EffectiveAddress.c
  C/CpuModule_Exceptions.c
  C/CpuModule_Flags.c
  C/CpuModule_Instructions.c
  C/CpuModule_InternalState.c
  C/CpuModule_Interrupts.c
  C/CpuModule_Logging.c
  C/CpuModule_StackFrameGen.c
  C/FellowList.cpp
  C/FellowMain.cpp
  C/FilesystemIntegration.cpp
  C/FloppyDisk.cpp
  C/Gameports.cpp
  C/GraphicsPipeline.cpp
  C/Keyboard.cpp
  C/LegacyCopper.cpp
  C/LineExactCopper.cpp
  C/LineExactSprites.cpp
  C/MemoryInterface.cpp
  C/Renderer.cpp
  C/Sound.cpp
  C/SpriteMerger.cpp
  C/SpriteP2CDecoder.cpp
  C/SpriteRegisters.cpp
  C/Sprites.cpp
  C/WavFileWriter.cpp
  C/chipset.cpp
  C/draw_interlace_control.cpp
  C/draw_pixelrenderers.cpp
  C/interrupt.c
  C/modrip.c
  C/rtc.cpp
  C/uart.cpp
  C/xdmswrap.c
  C/zlibwrap.c
  automation/Automator.cpp
  automation/Script.cpp
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
  ../WinFellow.Core/DebugApi/DebugVM.cpp
  ../WinFellow.Core/IO/RtcOkiMsm6242rs.cpp
  ../WinFellow.Core/VirtualHost/Core.cpp
  VirtualHost/CoreFactory.cpp
  zlib/C/crc32.c
)

# Frontends and runtime smoke tests consume the portable core through the
# namespaced target. Keep the concrete target name compatible with the legacy
# project layout while exposing a stable CMake interface to new components.
add_library(FellowNG::Emulation.Portable ALIAS FellowNG.Emulation.Portable)

set_source_files_properties(
  C/CpuIntegration.c
  C/CpuModule.c
  C/CpuModule_Disassembler.c
  C/CpuModule_EffectiveAddress.c
  C/CpuModule_Exceptions.c
  C/CpuModule_Flags.c
  C/CpuModule_Instructions.c
  C/CpuModule_InternalState.c
  C/CpuModule_Interrupts.c
  C/CpuModule_Logging.c
  C/CpuModule_StackFrameGen.c
  C/interrupt.c
  PROPERTIES LANGUAGE CXX
)

target_include_directories(FellowNG.Emulation.Portable PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/automation
  ${CMAKE_CURRENT_SOURCE_DIR}/graphics
  ${CMAKE_CURRENT_SOURCE_DIR}/uae
  ${CMAKE_CURRENT_SOURCE_DIR}/xdms/include
  ${CMAKE_CURRENT_SOURCE_DIR}/zlib/include
  ${CMAKE_CURRENT_SOURCE_DIR}/zlibwrap/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core
  ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Hardfile
)

if(MSVC)
  target_compile_definitions(FellowNG.Emulation.Portable PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_include_directories(FellowNG.Emulation.Portable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portable/include
  )

  set_source_files_properties(C/GraphicsPipeline.cpp PROPERTIES
    COMPILE_OPTIONS "-fpermissive"
  )

  set_source_files_properties(C/CpuModule_Instructions.c PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableCpuCompat.h"
  )

  set_source_files_properties(C/Renderer.cpp PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableRendererCompat.h"
  )

  set_source_files_properties(C/FloppyDisk.cpp PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableCrtCompat.h"
  )

  # The legacy memory translation unit defines its hot public read helpers as
  # __inline in the .cpp file. MSVC emitted externally callable definitions,
  # while GCC may discard them because they are not used in that translation
  # unit. Retain those definitions until the memory API is moved into a proper
  # portable header/implementation split.
  set_source_files_properties(C/MemoryInterface.cpp PROPERTIES
    COMPILE_OPTIONS "-fkeep-inline-functions"
  )
endif()
