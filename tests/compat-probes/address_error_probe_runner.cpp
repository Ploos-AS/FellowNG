#include <csetjmp>
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
std::jmp_buf mid_instruction_env;
bool mid_instruction_armed = false;
void no_interrupts() {}
void reset_exception() {}
void mid_instruction() {
  if (mid_instruction_armed) std::longjmp(mid_instruction_env, 1);
}
char marker(uint32_t address) {
  const uint8_t value = memoryReadByte(address);
  return value >= 0x20 && value <= 0x7e ? static_cast<char>(value) : '.';
}
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: fellowng-address-error-probe-runner <raw-image>\n";
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
  bool caught = false;
  uint32_t instructions = 0;
  uint32_t first_pc = cpuGetPC();
  uint32_t last_pc = first_pc;
  uint16_t last_opcode = 0;
  for (; instructions < 64; ++instructions) {
    last_pc = cpuGetPC();
    last_opcode = memoryReadWord(last_pc);
    mid_instruction_armed = true;
    if (setjmp(mid_instruction_env) == 0) {
      cpuExecuteInstruction();
      mid_instruction_armed = false;
    } else {
      mid_instruction_armed = false;
      caught = true;
    }
    if (caught) break;
  }

  // Exception 3 must have transferred control to the handler before the
  // mid-instruction escape. Execute one handler instruction to record 'A'.
  if (caught) cpuExecuteInstruction();

  const uint32_t sp = cpuGetAReg(7);
  const uint32_t stacked_fault_address = memoryReadLong(sp + 2);
  const uint32_t stacked_pc = memoryReadLong(sp + 8);
  const uint16_t stacked_sr = memoryReadWord(sp + 12);
  const uint16_t status_word = memoryReadWord(sp);
  const char m = marker(result);
  const bool pass = caught && m == 'A' && memory_fault_address == 0x1011 &&
                    stacked_fault_address == 0x1011;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"m68000-address-error-v1\","
            << "\"profile\":\"m68000-bare-metal\","
            << "\"subsystem\":\"exceptions\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":[{\"name\":\"instruction_count\",\"value\":" << instructions + 1
            << "},{\"name\":\"reset_pc\",\"value\":" << reset_pc
            << "},{\"name\":\"first_pc\",\"value\":" << first_pc
            << "},{\"name\":\"last_pc\",\"value\":" << last_pc
            << "},{\"name\":\"last_opcode\",\"value\":" << last_opcode
            << "},{\"name\":\"caught_mid_instruction\",\"value\":" << (caught ? "true" : "false")
            << "},{\"name\":\"vector_address_error\",\"value\":" << memoryReadLong(0x0c)
            << "},{\"name\":\"fault_address\",\"value\":" << memory_fault_address
            << "},{\"name\":\"fault_read\",\"value\":" << (memory_fault_read ? "true" : "false")
            << "},{\"name\":\"stack_pointer\",\"value\":" << sp
            << "},{\"name\":\"stack_status_word\",\"value\":" << status_word
            << "},{\"name\":\"stack_fault_address\",\"value\":" << stacked_fault_address
            << "},{\"name\":\"stack_pc\",\"value\":" << stacked_pc
            << "},{\"name\":\"stack_sr\",\"value\":" << stacked_sr
            << "},{\"name\":\"markers\",\"value\":\"" << m << "\"}]}\n";

  memoryShutdown();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
