find_package(SDL3 REQUIRED CONFIG)

add_executable(fellowng-sdl
  main.cpp
  SdlVideoOutput.cpp
  SdlInputSource.cpp
  SdlAudioOutput.cpp
  SdlFrontendSession.cpp
  $<TARGET_OBJECTS:FellowNG.Emulation.Portable>
)

add_executable(fellowng-sdl-runtime-factory-smoke
  runtime_factory_smoke.cpp
  SdlFrontendSession.cpp
  $<TARGET_OBJECTS:FellowNG.Emulation.Portable>
)

target_compile_features(fellowng-sdl PRIVATE cxx_std_20)
target_compile_features(fellowng-sdl-runtime-factory-smoke PRIVATE cxx_std_20)

target_link_libraries(fellowng-sdl
  PRIVATE
    FellowNG.Core.Portable
    SDL3::SDL3
)

target_link_libraries(fellowng-sdl-runtime-factory-smoke
  PRIVATE FellowNG.Core.Portable
)

add_test(
  NAME sdl-runtime-factory-smoke
  COMMAND fellowng-sdl-runtime-factory-smoke
)

if(MSVC)
  target_compile_options(fellowng-sdl PRIVATE /W4)
  target_compile_options(fellowng-sdl-runtime-factory-smoke PRIVATE /W4)
else()
  target_compile_options(fellowng-sdl PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(fellowng-sdl-runtime-factory-smoke PRIVATE -Wall -Wextra -Wpedantic)
endif()
