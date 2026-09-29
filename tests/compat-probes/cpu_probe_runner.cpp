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
    std::cerr << "usage: fellowng-cpu-probe-runner <raw-image>\n";
    return 2;
  }

  std::ifstream in(argv[1], std::ios::binary);
  std::vector<uint8_t> image((std::istreambuf_iterator<char>(in)), {});
  if (!in.good() && !in.eof()) return 2;
  if (image.empty() || image.size() > 512 * 1024) return 2;

  CoreFactory::CreateServices();
  memoryStartup();
  memorySetChipSize(512 * 1024);
  memoryHardReset();
  // Bare-metal probes execute from chip RAM at address zero; disable the
  // normal Kickstart overlay before loading vectors and code.
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
  cpuInitializeFromNewPC(0x00000400);

  constexpr uint32_t result = 0x1000;
  bool pass = false;
  uint32_t instructions = 0;
  uint32_t last_pc = cpuGetPC();
  uint32_t stagnant_pc_count = 0;
  uint32_t trap_opcode_pc = 0xffffffff;
  uint32_t trap_dispatch_pc = 0xffffffff;
  uint32_t trap_handler_pc = 0xffffffff;
  uint32_t trap_return_pc = 0xffffffff;
  uint32_t divzero_return_pc = 0xffffffff;
  uint32_t first_pc_after_divzero_marker = 0xffffffff;
  for (; instructions < 10000; ++instructions) {
    const uint32_t pc_before = cpuGetPC();
    cpuExecuteInstruction();
    const uint32_t opcode_pc = cpuGetOriginalPC();
    const uint16_t dispatched_opcode = cpuGetLastDispatchedOpcode();

    const uint32_t pc = cpuGetPC();
    if (dispatched_opcode == 0x4e73 && memoryReadByte(result + 1) == 'D' && divzero_return_pc == 0xffffffff) {
      divzero_return_pc = pc;
    } else if (memoryReadByte(result + 1) == 'D' && first_pc_after_divzero_marker == 0xffffffff) {
      first_pc_after_divzero_marker = opcode_pc;
    }
    if (dispatched_opcode == 0x4e40 && trap_opcode_pc == 0xffffffff) {
      trap_opcode_pc = opcode_pc;
      trap_dispatch_pc = pc;
    } else if (trap_dispatch_pc != 0xffffffff && trap_handler_pc == 0xffffffff) {
      trap_handler_pc = pc;
    } else if (trap_handler_pc != 0xffffffff && trap_return_pc == 0xffffffff) {
      trap_return_pc = pc;
    }
    if (pc == last_pc) {
      ++stagnant_pc_count;
    } else {
      stagnant_pc_count = 0;
      last_pc = pc;
    }

    if (memoryReadByte(result) == 'I' &&
        memoryReadByte(result + 1) == 'D' &&
        memoryReadByte(result + 2) == 'T' &&
        memoryReadByte(result + 3) == 'P') {
      pass = true;
      break;
    }

    // A bare-metal probe should always make forward progress except for its
    // terminal loop. Stop early on a stuck CPU so CI reports the useful PC
    // and partial exception markers instead of burning all 10k instructions.
    if (stagnant_pc_count >= 32) break;
  }

  const char m0 = marker(result);
  const char m1 = marker(result + 1);
  const char m2 = marker(result + 2);
  const char m3 = marker(result + 3);

  const char *stage = "entry";
  if (m0 == 'I') stage = "after-illegal";
  if (m0 == 'I' && m1 == 'D') stage = "after-divzero";
  if (m0 == 'I' && m1 == 'D' && m2 == 'T') stage = "after-trap";
  if (m0 == 'I' && m1 == 'D' && m2 == 'T' && m3 == 'P') stage = "complete";

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"m68000-exception-vectors-v1\","
            << "\"profile\":\"m68000-bare-metal\","
            << "\"subsystem\":\"exceptions\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":[{\"name\":\"instruction_count\",\"value\":" << instructions + 1
            << "},{\"name\":\"pc\",\"value\":" << cpuGetPC()
            << "},{\"name\":\"entry_word\",\"value\":" << memoryReadWord(0x400)
            << ",\"vector_illegal\":" << memoryReadLong(0x10)
            << ",\"vector_divzero\":" << memoryReadLong(0x14)
            << ",\"vector_privilege\":" << memoryReadLong(0x20)
            << ",\"vector_trap0\":" << memoryReadLong(0x80)
            << ",\"trap_site_word\":" << memoryReadWord(0x420)
            << ",\"last_dispatched_opcode\":" << cpuGetLastDispatchedOpcode()
            << ",\"divzero_return_pc\":" << divzero_return_pc
            << ",\"first_pc_after_divzero_marker\":" << first_pc_after_divzero_marker
            << "},{\"name\":\"stage\",\"value\":\"" << stage << "\""
            << ",\"stagnant_pc_count\":" << stagnant_pc_count
            << ",\"trap_opcode_pc\":" << trap_opcode_pc
            << ",\"trap_dispatch_pc\":" << trap_dispatch_pc
            << ",\"trap_handler_pc\":" << trap_handler_pc
            << ",\"trap_return_pc\":" << trap_return_pc
            << "},{\"name\":\"markers\",\"value\":\"" << m0 << m1 << m2 << m3
            << "\"}]}\n";

  memoryShutdown();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
