#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "Defs.h"
#include "CpuModule.h"
#include "CpuIntegration.h"
#include "MemoryInterface.h"
#include "VirtualHost/CoreFactory.h"

namespace {
void no_interrupts() {}
void mid_instruction() {}
void reset_exception() {}
char marker(uint32_t address) {
  const uint8_t value = memoryReadByte(address);
  return value >= 0x20 && value <= 0x7e ? static_cast<char>(value) : '.';
}
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: fellowng-interrupt-probe-runner <raw-image>\n";
    return 2;
  }
  std::ifstream in(argv[1], std::ios::binary);
  std::vector<uint8_t> image((std::istreambuf_iterator<char>(in)), {});
  if (image.empty() || image.size() > 512 * 1024) return 2;

  CoreFactory::CreateServices();
  memoryStartup();
  memorySetChipSize(512 * 1024);
  memoryHardReset();
  memoryChipMap(false);
  cpuIntegrationStartup();
  cpuSetModel(0, 0);
  cpuSetCheckPendingInterruptsFunc(no_interrupts);
  cpuSetMidInstructionExceptionFunc(mid_instruction);
  cpuSetResetExceptionFunc(reset_exception);
  for (uint32_t i = 0; i < image.size(); ++i) memoryWriteByte(image[i], i);

  cpuSetAReg(7, 0x0007fff0);
  cpuSetSspDirect(0x0007fff0);
  cpuSetUspDirect(0x0007f000);
  cpuSetSR(0x2700);
  const uint32_t reset_pc = memoryReadLong(4);
  cpuInitializeFromNewPC(reset_pc);

  constexpr uint32_t result = 0x1000;
  // Execute LEA/CLR/CLR/MOVE-to-SR so the guest is waiting in user mode.
  for (int i = 0; i < 4; ++i) cpuExecuteInstruction();
  const uint32_t user_sp_before = cpuGetAReg(7);
  const uint32_t ssp_before = cpuGetSspDirect();
  const uint32_t pc_before_interrupt = cpuGetPC();

  cpuSetIrqLevel(3);
  cpuExecuteInstruction(); // interrupt entry
  const uint32_t handler_pc = cpuGetPC();
  const uint32_t handler_sp = cpuGetAReg(7);
  const uint32_t handler_sr = cpuGetSR();
  const uint16_t stacked_sr = memoryReadWord(handler_sp);
  const uint32_t stacked_pc = memoryReadLong(handler_sp + 2);

  cpuExecuteInstruction(); // marker I
  cpuExecuteInstruction(); // RTE
  const uint32_t return_pc = cpuGetPC();
  const uint32_t return_sp = cpuGetAReg(7);
  const uint32_t return_sr = cpuGetSR();

  const char m0 = marker(result);
  const bool pass = m0 == 'I' &&
                    memoryReadLong(0x6c) == handler_pc &&
                    (handler_sr & 0x2000) != 0 &&
                    ((handler_sr >> 8) & 7) == 3 &&
                    stacked_pc == pc_before_interrupt &&
                    return_pc == pc_before_interrupt &&
                    return_sp == user_sp_before &&
                    (return_sr & 0x2000) == 0;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"m68000-interrupt-level3-v1\","
            << "\"profile\":\"m68000-bare-metal\","
            << "\"subsystem\":\"interrupts\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":[{\"name\":\"vector_level3\",\"value\":" << memoryReadLong(0x6c)
            << "},{\"name\":\"pc_before_interrupt\",\"value\":" << pc_before_interrupt
            << "},{\"name\":\"handler_pc\",\"value\":" << handler_pc
            << "},{\"name\":\"user_sp_before\",\"value\":" << user_sp_before
            << "},{\"name\":\"ssp_before\",\"value\":" << ssp_before
            << "},{\"name\":\"handler_sp\",\"value\":" << handler_sp
            << "},{\"name\":\"handler_sr\",\"value\":" << handler_sr
            << "},{\"name\":\"stacked_sr\",\"value\":" << stacked_sr
            << "},{\"name\":\"stacked_pc\",\"value\":" << stacked_pc
            << "},{\"name\":\"return_pc\",\"value\":" << return_pc
            << "},{\"name\":\"return_sp\",\"value\":" << return_sp
            << "},{\"name\":\"return_sr\",\"value\":" << return_sr
            << "},{\"name\":\"markers\",\"value\":\"" << m0 << "\"}]}\n";

  memoryShutdown();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
