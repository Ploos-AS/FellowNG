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
