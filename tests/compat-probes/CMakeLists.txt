add_executable(fellowng-cpu-probe-runner
  cpu_probe_runner.cpp
)
target_link_libraries(fellowng-cpu-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-cpu-probe-runner PRIVATE cxx_std_20)

add_executable(fellowng-address-error-probe-runner
  address_error_probe_runner.cpp
)
target_link_libraries(fellowng-address-error-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-address-error-probe-runner PRIVATE cxx_std_20)
