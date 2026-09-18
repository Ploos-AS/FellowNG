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
  uae/AUTOCONF.C
  uae/EXPANSIO.C
  uae/FILESYS.C
  uae/FSUSAGE.C
  uae/HARDFILE.C
  uae/UAESUPP.C
  uae/fsdb.c
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
  ../WinFellow.Core/CustomChipset/RegisterUtility.cpp
  ../WinFellow.Core/CustomChipset/Registers.cpp
  ../WinFellow.Core/DebugApi/DebugVM.cpp
  DebugApi/M68K.cpp
  DebugApi/MemorySystem.cpp
  ../WinFellow.Core/Module/Hardfile/HardfileConfiguration.cpp
  ../WinFellow.Core/Module/Hardfile/HardfileGeometry.cpp
  ../WinFellow.Core/Service/FileInformation.cpp
  ../WinFellow.Core/Service/Log.cpp
  ../WinFellow.Core/IO/RtcOkiMsm6242rs.cpp
  ../WinFellow.Core/VirtualHost/Core.cpp
  VirtualHost/CoreFactory.cpp
  ../WinFellow.Hardfile/hardfile/HardfileHandler.cpp
  ../WinFellow.Hardfile/hardfile/HardfileDevice.cpp
  ../WinFellow.Hardfile/hardfile/HardfileFileSystemEntry.cpp
  ../WinFellow.Hardfile/hardfile/HardfileMountListEntry.cpp
  ../WinFellow.Hardfile/hardfile/hunks/AdditionalHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/BSSHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/CodeHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/DataHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/EndHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/FileImage.cpp
  ../WinFellow.Hardfile/hardfile/hunks/HeaderHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/HunkFactory.cpp
  ../WinFellow.Hardfile/hardfile/hunks/HunkParser.cpp
  ../WinFellow.Hardfile/hardfile/hunks/HunkRelocator.cpp
  ../WinFellow.Hardfile/hardfile/hunks/HunkSize.cpp
  ../WinFellow.Hardfile/hardfile/hunks/InitialHunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/RawDataReader.cpp
  ../WinFellow.Hardfile/hardfile/hunks/Reloc32Hunk.cpp
  ../WinFellow.Hardfile/hardfile/hunks/Reloc32OffsetTable.cpp
  ../WinFellow.Hardfile/hardfile/rdb/ChecksumCalculator.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDB.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBFileReader.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBFileSystemHandler.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBFileSystemHeader.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBHandler.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBLSegBlock.cpp
  ../WinFellow.Hardfile/hardfile/rdb/RDBPartition.cpp
  uae/AUTOCONF.C
  uae/EXPANSIO.C
  uae/FILESYS.C
  uae/FSUSAGE.C
  uae/HARDFILE.C
  uae/UAESUPP.C
  zlib/C/adler32.c
  zlib/C/compress.c
  zlib/C/crc32.c
  zlib/C/deflate.c
  zlib/C/gzclose.c
  zlib/C/gzlib.c
  zlib/C/gzread.c
  zlib/C/gzwrite.c
  zlib/C/infback.c
  zlib/C/inffast.c
  zlib/C/inflate.c
  zlib/C/inftrees.c
  zlib/C/trees.c
  zlib/C/uncompr.c
  zlib/C/zutil.c
  xdms/C/crc_csum.c
  xdms/C/getbits.c
  xdms/C/maketbl.c
  xdms/C/pfile.c
  xdms/C/tables.c
  xdms/C/u_deep.c
  xdms/C/u_heavy.c
  xdms/C/u_init.c
  xdms/C/u_medium.c
  xdms/C/u_quick.c
  xdms/C/u_rle.c
  xdms/C/xdms.c
)

# Frontends and runtime smoke tests consume the portable core through the
# namespaced target. Keep the concrete target name compatible with the legacy
# project layout while exposing a stable CMake interface to new components.
add_library(FellowNG::Emulation.Portable ALIAS FellowNG.Emulation.Portable)

target_compile_features(FellowNG.Emulation.Portable PUBLIC cxx_std_20)

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
  C/modrip.c
  C/xdmswrap.c
  C/zlibwrap.c
  uae/fsdb.c
  xdms/C/pfile.c
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