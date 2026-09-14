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
