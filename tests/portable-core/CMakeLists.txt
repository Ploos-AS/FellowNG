add_executable(fellowng-portable-core-smoke
  portable_core_smoke.cpp
)

target_link_libraries(fellowng-portable-core-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-portable-core-smoke PRIVATE cxx_std_20)

add_test(
  NAME portable-core-smoke
  COMMAND fellowng-portable-core-smoke
)

add_executable(fellowng-runtime-session-smoke
  runtime_session_smoke.cpp
)

target_link_libraries(fellowng-runtime-session-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-runtime-session-smoke PRIVATE cxx_std_20)

add_test(
  NAME runtime-session-smoke
  COMMAND fellowng-runtime-session-smoke
)

add_executable(fellowng-platform-factory-contract-smoke
  platform_factory_contract_smoke.cpp
)

target_link_libraries(fellowng-platform-factory-contract-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-platform-factory-contract-smoke PRIVATE cxx_std_20)

add_test(
  NAME platform-factory-contract-smoke
  COMMAND fellowng-platform-factory-contract-smoke
)

add_executable(fellowng-portable-sound-driver-smoke
  portable_sound_driver_smoke.cpp
)

target_link_libraries(fellowng-portable-sound-driver-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-portable-sound-driver-smoke PRIVATE cxx_std_20)

add_test(
  NAME portable-sound-driver-smoke
  COMMAND fellowng-portable-sound-driver-smoke
)

add_executable(fellowng-portable-audio-platform-factory-smoke
  portable_audio_platform_factory_smoke.cpp
)

target_link_libraries(fellowng-portable-audio-platform-factory-smoke
  PRIVATE FellowNG.Core.Portable
)
target_compile_features(fellowng-portable-audio-platform-factory-smoke PRIVATE cxx_std_20)

add_test(
  NAME portable-audio-platform-factory-smoke
  COMMAND fellowng-portable-audio-platform-factory-smoke
)

add_executable(fellowng-module-lifecycle-smoke
  module_lifecycle_smoke.cpp
)

target_link_libraries(fellowng-module-lifecycle-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-module-lifecycle-smoke PRIVATE cxx_std_20)

add_test(
  NAME module-lifecycle-smoke
  COMMAND fellowng-module-lifecycle-smoke
)

add_executable(fellowng-runtime-factory-smoke
  runtime_factory_smoke.cpp
)

target_link_libraries(fellowng-runtime-factory-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-runtime-factory-smoke PRIVATE cxx_std_20)

add_test(
  NAME runtime-factory-smoke
  COMMAND fellowng-runtime-factory-smoke
)

add_executable(fellowng-portable-core-platform-factory-smoke
  portable_core_platform_factory_smoke.cpp
)

target_link_libraries(fellowng-portable-core-platform-factory-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-portable-core-platform-factory-smoke PRIVATE cxx_std_20)

add_test(
  NAME portable-core-platform-factory-smoke
  COMMAND fellowng-portable-core-platform-factory-smoke
)

add_executable(fellowng-portable-bootstrap-platform-factory-smoke
  portable_bootstrap_platform_factory_smoke.cpp
)

target_link_libraries(fellowng-portable-bootstrap-platform-factory-smoke
  PRIVATE FellowNG.Core.Portable
)

target_compile_features(fellowng-portable-bootstrap-platform-factory-smoke PRIVATE cxx_std_20)

add_test(
  NAME portable-bootstrap-platform-factory-smoke
  COMMAND fellowng-portable-bootstrap-platform-factory-smoke
)
