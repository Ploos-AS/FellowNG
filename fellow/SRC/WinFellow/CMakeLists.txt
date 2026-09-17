cmake_minimum_required(VERSION 3.20)

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
  C/rtc.cpp
  C/uart.cpp
  C/xdmswrap.c
  C/zlibwrap.c
  automation/Automator.cpp
  automation/Script.cpp
  ../WinFellow.Core/Core/Core.cpp
  ../WinFellow.Core/Core/CoreFactory.cpp
  ../WinFellow.Core/DebugApi/DebugVM.cpp
  ../WinFellow.Core/IO/RtcOkiMsm6242rs.cpp
  ../WinFellow.Core/Service/PortableFileops.cpp
  ../WinFellow.Core/Service/PortableSystemInformation.cpp
  ../WinFellow.Core/Service/PortableThreading.cpp
  zlib/C/crc32.c
)

target_include_directories(FellowNG.Emulation.Portable
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core/Driver
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core/Service
    ${CMAKE_CURRENT_SOURCE_DIR}/../WinFellow.Core/VirtualHost
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/automation
    ${CMAKE_CURRENT_SOURCE_DIR}/xdms/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zlibwrap/include
)

target_compile_features(FellowNG.Emulation.Portable PUBLIC cxx_std_17)

if(MSVC)
  target_compile_definitions(FellowNG.Emulation.Portable PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_include_directories(FellowNG.Emulation.Portable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portable/include
  )

  # Legacy graphics code still contains a small number of pointer assignments
  # accepted by the original MSVC build. Keep this workaround narrowly scoped
  # until the callback cleanup is completed.
  set_source_files_properties(C/GraphicsPipeline.cpp PROPERTIES
    COMPILE_OPTIONS "-fpermissive"
  )

  # The legacy CPU sources use C++ headers/types despite their .c suffixes.
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

  # Keep the temporary host-trap compatibility local to the instruction core.
  set_source_files_properties(C/CpuModule_Instructions.c PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableCpuCompat.h"
  )

  # Keep the renderer host compatibility local to the renderer implementation.
  set_source_files_properties(C/Renderer.cpp PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableRendererCompat.h"
  )

  # Keep the MSVC secure-CRT compatibility local to floppy handling.
  set_source_files_properties(C/FloppyDisk.cpp PROPERTIES
    COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE/PortableCrtCompat.h"
  )
endif()

add_library(FellowNG::Emulation.Portable ALIAS FellowNG.Emulation.Portable)
