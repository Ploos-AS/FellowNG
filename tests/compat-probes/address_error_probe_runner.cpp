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
  // cpuStartup clears the IRQ level through cpuSetIrqLevel(0), which leaves
  // the legacy raise-interrupt latch set. Bare-metal probes start quiescent.
  cpuSetRaiseInterrupt(FALSE);
  cpuSetMidInstructionExceptionFunc(mid_instruction);
  cpuSetResetExceptionFunc(reset_exception);
  for (uint32_t i = 0; i < image.size(); ++i) memoryWriteByte(image[i], i);

  cpuSetAReg(7, 0x0007fff0);
  cpuSetSspDirect(0x0007fff0);
  cpuSetUspDirect(0x0007f000);
  cpuSetSR(0x2700);
  const uint32_t reset_pc = memoryReadLong(4);
  const uint16_t entry_word_before_init = memoryReadWord(reset_pc);
  cpuInitializeFromNewPC(reset_pc);
  const uint32_t pc_after_init = cpuGetPC();
  const uint16_t prefetch_after_init = cpuGetPrefetchWord();

  constexpr uint32_t result = 0x1000;
  bool caught = false;
  uint32_t instructions = 0;
  uint32_t first_pc = cpuGetPC();
  uint32_t last_pc = first_pc;
  uint16_t last_opcode = 0;
  uint32_t trace_pc[12] = {};
  uint16_t trace_opcode[12] = {};
  uint32_t trace_count = 0;
  uint32_t a0_after_first = 0xffffffff;
  uint32_t pc_after_first = 0xffffffff;
  uint16_t dispatched_after_first = 0xffff;
  uint16_t prefetch_before_first_execute = 0xffff;
  for (; instructions < 64; ++instructions) {
    last_pc = cpuGetPC();
    last_opcode = memoryReadWord(last_pc);
    if (trace_count < 12) {
      trace_pc[trace_count] = last_pc;
      trace_opcode[trace_count] = last_opcode;
      ++trace_count;
    }
    if (setjmp(mid_instruction_env) == 0) {
      if (instructions == 0) prefetch_before_first_execute = cpuGetPrefetchWord();
      mid_instruction_armed = true;
      cpuExecuteInstruction();
      if (instructions == 0) {
        a0_after_first = cpuGetAReg(0);
        pc_after_first = cpuGetPC();
        dispatched_after_first = cpuGetLastDispatchedOpcode();
      }
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
  // Fellow's 68000 Group-2 frame is laid out from SP as:
  // status word, reserved/ireg word, fault address, SR, PC.
  const uint16_t status_word = memoryReadWord(sp);
  const uint32_t stacked_fault_address = memoryReadLong(sp + 4);
  const uint16_t stacked_sr = memoryReadWord(sp + 8);
  const uint32_t stacked_pc = memoryReadLong(sp + 10);
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
            << "},{\"name\":\"entry_word_before_init\",\"value\":" << entry_word_before_init
            << "},{\"name\":\"pc_after_init\",\"value\":" << pc_after_init
            << "},{\"name\":\"prefetch_after_init\",\"value\":" << prefetch_after_init
            << "},{\"name\":\"first_pc\",\"value\":" << first_pc
            << "},{\"name\":\"last_pc\",\"value\":" << last_pc
            << "},{\"name\":\"last_opcode\",\"value\":" << last_opcode
            << "},{\"name\":\"trace\",\"value\":\"";
  for (uint32_t i = 0; i < trace_count; ++i) {
    if (i) std::cout << ",";
    std::cout << trace_pc[i] << ":" << trace_opcode[i];
  }
  std::cout << "\""
            << "},{\"name\":\"prefetch_before_first_execute\",\"value\":" << prefetch_before_first_execute
            << "},{\"name\":\"a0_after_first\",\"value\":" << a0_after_first
            << "},{\"name\":\"pc_after_first\",\"value\":" << pc_after_first
            << "},{\"name\":\"dispatched_after_first\",\"value\":" << dispatched_after_first
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
