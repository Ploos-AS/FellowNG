add_executable(fellowng-cpu-probe-runner
  cpu_probe_runner.cpp
)
target_link_libraries(fellowng-cpu-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-cpu-probe-runner PRIVATE cxx_std_20)
