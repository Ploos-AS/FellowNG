#include <cstdint>
#include <iostream>

#include "Blitter.h"
#include "BusScheduler.h"
#include "CopperRegisters.h"
#include "LegacyCopper.h"
#include "MemoryInterface.h"
#include "VirtualHost/Core.h"
#include "chipset.h"

namespace {
uint16_t observed_color00 = 0;
uint32_t color00_writes = 0;

void capture_color00(uint16_t data, uint32_t) {
  observed_color00 = data;
  ++color00_writes;
}

void execute_copper_event(uint32_t cycle) {
  bus.cycle = cycle;
  copperEvent.handler();
}
}

int main() {
  constexpr uint32_t list = 0x1000;

  chipsetStartup();
  busStartup();
  busHardReset();
  memoryStartup();
  blitterStartup();
  copperStartup();

  // Project-authored Copper list:
  //   MOVE #$0123,COLOR00
  //   WAIT v=0,h=$20
  //   MOVE #$0456,COLOR00
  //   end
  chipmemWriteWord(0x0180, list + 0);
  chipmemWriteWord(0x0123, list + 2);
  chipmemWriteWord(0x0021, list + 4);
  chipmemWriteWord(0xfffe, list + 6);
  chipmemWriteWord(0x0180, list + 8);
  chipmemWriteWord(0x0456, list + 10);
  chipmemWriteWord(0xffff, list + 12);
  chipmemWriteWord(0xfffe, list + 14);

  memorySetIoWriteStub(0x180, capture_color00);
  _core.Registers.BplCon0 = 0;

  copper_registers.copper_dma = true;
  copper->Load(list);

  const uint32_t first_cycle = copperEvent.cycle;
  execute_copper_event(first_cycle);
  const uint16_t first_color = observed_color00;
  const uint32_t after_move_pc = copper_registers.copper_pc;
  const uint32_t wait_decode_cycle = copperEvent.cycle;

  execute_copper_event(wait_decode_cycle);
  const uint32_t wait_target_cycle = copperEvent.cycle;
  const uint32_t after_wait_pc = copper_registers.copper_pc;

  execute_copper_event(wait_target_cycle);
  const uint32_t second_move_cycle = copperEvent.cycle;
  execute_copper_event(second_move_cycle);
  const uint16_t second_color = observed_color00;
  const uint32_t after_second_move_pc = copper_registers.copper_pc;

  const uint32_t end_cycle = copperEvent.cycle;
  execute_copper_event(end_cycle);
  const uint32_t final_pc = copper_registers.copper_pc;
  const uint32_t final_event_cycle = copperEvent.cycle;

  const bool pass =
      first_cycle == 4 &&
      first_color == 0x0123 &&
      after_move_pc == list + 4 &&
      wait_decode_cycle == 8 &&
      wait_target_cycle > wait_decode_cycle &&
      after_wait_pc == list + 8 &&
      second_color == 0x0456 &&
      color00_writes == 2 &&
      after_second_move_pc == list + 12 &&
      final_pc == list + 16 &&
      final_event_cycle == BUS_CYCLE_DISABLE;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"copper-move-wait-end-v1\","
            << "\"profile\":\"ocs-line-exact-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"first_cycle\",\"value\":" << first_cycle << "},"
            << "{\"name\":\"first_color\",\"value\":" << first_color << "},"
            << "{\"name\":\"wait_decode_cycle\",\"value\":" << wait_decode_cycle << "},"
            << "{\"name\":\"wait_target_cycle\",\"value\":" << wait_target_cycle << "},"
            << "{\"name\":\"second_color\",\"value\":" << second_color << "},"
            << "{\"name\":\"color00_writes\",\"value\":" << color00_writes << "},"
            << "{\"name\":\"final_pc\",\"value\":" << final_pc << "},"
            << "{\"name\":\"final_event_cycle\",\"value\":" << final_event_cycle << "}"
            << "]}\n";

  copperShutdown();
  blitterShutdown();
  memoryShutdown();
  busShutdown();
  return pass ? 0 : 1;
}
