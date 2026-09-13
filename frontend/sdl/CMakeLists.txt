find_package(SDL3 REQUIRED CONFIG)

add_executable(fellowng-sdl
  main.cpp
)

target_compile_features(fellowng-sdl PRIVATE cxx_std_20)

target_link_libraries(fellowng-sdl
  PRIVATE
    FellowNG.Core.Portable
    SDL3::SDL3
)

if(MSVC)
  target_compile_options(fellowng-sdl PRIVATE /W4)
else()
  target_compile_options(fellowng-sdl PRIVATE -Wall -Wextra -Wpedantic)
endif()
