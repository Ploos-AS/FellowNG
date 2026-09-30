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
    std::cerr << "usage: fellowng-68020-probe-runner <raw-image>\n";
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
  cpuSetModel(2, 0);
  cpuSetCheckPendingInterruptsFunc(no_interrupts);
  // Bare-metal probes begin without the legacy level-0 IRQ latch left by startup.
  cpuSetRaiseInterrupt(FALSE);
  cpuSetMidInstructionExceptionFunc(mid_instruction);
  cpuSetResetExceptionFunc(reset_exception);
  for (uint32_t i = 0; i < image.size(); ++i) memoryWriteByte(image[i], i);

  cpuSetAReg(7, 0x0007fff0);
  cpuSetSspDirect(0x0007fff0);
  cpuSetUspDirect(0x0007f000);
  cpuSetSR(0x2700);
  const uint32_t reset_pc = memoryReadLong(4);
  cpuInitializeFromNewPC(reset_pc);

  constexpr uint32_t result = 0x1100;
  bool saw_handler = false;
  uint32_t handler_sp = 0;
  uint16_t format_vector = 0xffff;
  uint32_t stacked_pc = 0xffffffff;
  uint32_t vbr = 0;
  uint32_t trap_vector = 0;
  uint32_t trap_return_pc = 0xffffffff;
  uint32_t instructions = 0;

  for (; instructions < 64; ++instructions) {
    const uint16_t opcode = memoryReadWord(cpuGetPC());
    cpuExecuteInstruction();
    if (opcode == 0x4e7b) vbr = cpuGetVbr(); // MOVEC <reg>,VBR
    if (opcode == 0x4e40) {
      handler_sp = cpuGetAReg(7);
      format_vector = memoryReadWord(handler_sp + 6);
      stacked_pc = memoryReadLong(handler_sp + 2);
      trap_vector = memoryReadLong(vbr + 0x80);
    }
    if (marker(result) == 'V') saw_handler = true;
    if (saw_handler && cpuGetLastDispatchedOpcode() == 0x4e73) {
      trap_return_pc = cpuGetPC();
    }
    if (marker(result) == 'V' && marker(result + 1) == 'R') break;
  }

  const char m0 = marker(result);
  const char m1 = marker(result + 1);
  const bool pass = m0 == 'V' && m1 == 'R' &&
                    vbr != 0 && trap_vector != 0 &&
                    (format_vector & 0xf000) == 0x0000 &&
                    (format_vector & 0x0fff) == 0x0080 &&
                    trap_return_pc == stacked_pc;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"m68020-vbr-trap-frame-v1\","
            << "\"profile\":\"m68020-bare-metal\","
            << "\"subsystem\":\"exceptions\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":[{\"name\":\"instruction_count\",\"value\":" << instructions + 1
            << "},{\"name\":\"vbr\",\"value\":" << vbr
            << "},{\"name\":\"trap_vector\",\"value\":" << trap_vector
            << "},{\"name\":\"handler_sp\",\"value\":" << handler_sp
            << "},{\"name\":\"format_vector\",\"value\":" << format_vector
            << "},{\"name\":\"stacked_pc\",\"value\":" << stacked_pc
            << "},{\"name\":\"trap_return_pc\",\"value\":" << trap_return_pc
            << "},{\"name\":\"markers\",\"value\":\"" << m0 << m1 << "\"}]}\n";

  memoryShutdown();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
