#include <cstdint>
#include <iostream>

#include "chipset.h"
#include "CopperRegisters.h"

int main() {
  chipsetStartup();
  copper_registers.ClearState();

  const uint32_t ocs_masked = chipsetMaskPtr(0x001abcdf);
  wcop1lch(0x001a, 0xdff080);
  wcop1lcl(0xbcdf, 0xdff082);
  const uint32_t ocs_cop1lc = copper_registers.cop1lc;

  chipsetSetECS(true);
  const uint32_t ecs_masked = chipsetMaskPtr(0x001abcdf);
  wcop2lch(0x001a, 0xdff084);
  wcop2lcl(0xbcdf, 0xdff086);
  const uint32_t ecs_cop2lc = copper_registers.cop2lc;

  const bool pass =
      ocs_masked == 0x0002bcde &&
      ocs_cop1lc == 0x0002bcde &&
      ecs_masked == 0x001abcde &&
      ecs_cop2lc == 0x001abcde &&
      (ocs_cop1lc & 1u) == 0 &&
      (ecs_cop2lc & 1u) == 0;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"chipset-pointer-registers-v1\","
            << "\"profile\":\"ocs-ecs-register-core\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"ocs_masked\",\"value\":" << ocs_masked << "},"
            << "{\"name\":\"ocs_cop1lc\",\"value\":" << ocs_cop1lc << "},"
            << "{\"name\":\"ecs_masked\",\"value\":" << ecs_masked << "},"
            << "{\"name\":\"ecs_cop2lc\",\"value\":" << ecs_cop2lc << "}"
            << "]}\n";
  return pass ? 0 : 1;
}
