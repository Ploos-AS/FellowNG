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

add_executable(fellowng-interrupt-probe-runner
  interrupt_probe_runner.cpp
)
target_link_libraries(fellowng-interrupt-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-interrupt-probe-runner PRIVATE cxx_std_20)

add_executable(fellowng-68020-probe-runner
  m68020_probe_runner.cpp
)
target_link_libraries(fellowng-68020-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-68020-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-chipset-register-probe-runner
  chipset_register_probe_runner.cpp
)
target_link_libraries(fellowng-chipset-register-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-chipset-register-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-copper-execution-probe-runner
  copper_execution_probe_runner.cpp
)
target_link_libraries(fellowng-copper-execution-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-copper-execution-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-blitter-copy-probe-runner
  blitter_copy_probe_runner.cpp
)
target_link_libraries(fellowng-blitter-copy-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-blitter-copy-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-bitplane-dma-probe-runner
  bitplane_dma_probe_runner.cpp
)
target_link_libraries(fellowng-bitplane-dma-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-bitplane-dma-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-sprite-dma-probe-runner
  sprite_dma_probe_runner.cpp
)
target_link_libraries(fellowng-sprite-dma-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-sprite-dma-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-sprite-p2c-probe-runner
  sprite_p2c_probe_runner.cpp
)
target_link_libraries(fellowng-sprite-p2c-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-sprite-p2c-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-interrupt-dma-probe-runner
  interrupt_dma_probe_runner.cpp
)
target_link_libraries(fellowng-interrupt-dma-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-interrupt-dma-probe-runner PRIVATE cxx_std_20)


add_executable(fellowng-copper-bitplane-contention-probe-runner
  copper_bitplane_contention_probe_runner.cpp
)
target_link_libraries(fellowng-copper-bitplane-contention-probe-runner PRIVATE FellowNG::Emulation.Portable)
target_compile_features(fellowng-copper-bitplane-contention-probe-runner PRIVATE cxx_std_20)
