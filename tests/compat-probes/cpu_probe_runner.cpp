#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "Defs.h"
#include "CpuModule.h"
#include "MemoryInterface.h"
#include "VirtualHost/CoreFactory.h"

namespace {
void no_interrupts() {}
void mid_instruction() {}
void reset_exception() {}
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

  cpuStartup();
  cpuSetModel(0, 0);
  cpuSetCheckPendingInterruptsFunc(no_interrupts);
  cpuSetMidInstructionExceptionFunc(mid_instruction);
  cpuSetResetExceptionFunc(reset_exception);

  for (uint32_t i = 0; i < image.size(); ++i) memoryWriteByte(image[i], i);

  cpuSetAReg(7, 0x0007fff0);
  cpuSetSspDirect(0x0007fff0);
  cpuSetUspDirect(0x0007f000);
  cpuSetSR(0x2700);
  cpuSetPC(0x00000400);

  constexpr uint32_t result = 0x1000;
  bool pass = false;
  uint32_t instructions = 0;
  for (; instructions < 10000; ++instructions) {
    cpuExecuteInstruction();
    if (memoryReadByte(result) == 'I' &&
        memoryReadByte(result + 1) == 'D' &&
        memoryReadByte(result + 2) == 'T' &&
        memoryReadByte(result + 3) == 'P') {
      pass = true;
      break;
    }
  }

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"m68000-exception-vectors-v1\","
            << "\"subsystem\":\"exceptions\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":{\"instruction_count\":" << instructions + 1
            << ",\"pc\":" << cpuGetPC()
            << ",\"entry_word\":" << memoryReadWord(0x400)
            << ",\"vector_illegal\":" << memoryReadLong(0x10)
            << ",\"vector_trap0\":" << memoryReadLong(0x80)
            << ",\"markers\":\""
            << char(memoryReadByte(result)) << char(memoryReadByte(result + 1))
            << char(memoryReadByte(result + 2)) << char(memoryReadByte(result + 3))
            << "\"}}\n";

  memoryShutdown();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
