add_library(FellowNG.Core.Portable STATIC
  CustomChipset/Registers.cpp
  CustomChipset/RegisterUtility.cpp
  Platform/StdClock.cpp
  Platform/StdFileSystem.cpp
  Platform/StreamLogger.cpp
  Platform/HostLifecycle.cpp
)

target_include_directories(FellowNG.Core.Portable
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(FellowNG.Core.Portable PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(FellowNG.Core.Portable PRIVATE /W4)
else()
  target_compile_options(FellowNG.Core.Portable PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(FellowNG.Core.Portable PROPERTIES
  CXX_EXTENSIONS OFF
  OUTPUT_NAME fellowng-core-portable
)
