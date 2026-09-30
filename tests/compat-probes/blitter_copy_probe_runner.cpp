#include <cstdint>
#include <iostream>

#include "Blitter.h"
#include "BusScheduler.h"
#include "MemoryInterface.h"
#include "chipset.h"

int main() {
  constexpr uint32_t src = 0x2000;
  constexpr uint32_t dst = 0x2100;

  chipsetStartup();
  busStartup();
  busHardReset();
  memoryStartup();
  blitterStartup();

  chipmemWriteWord(0x1111, src + 0);
  chipmemWriteWord(0x2222, src + 2);
  chipmemWriteWord(0x3333, src + 4);
  chipmemWriteWord(0x4444, src + 6);
  for (uint32_t offset = 0; offset < 8; offset += 2) chipmemWriteWord(0, dst + offset);

  // A -> D, one word wide, four words high. Minterm 0xF0 selects A.
  wbltcon0(0x09f0, 0xdff040);
  wbltcon1(0x0000, 0xdff042);
  wbltafwm(0xffff, 0xdff044);
  wbltalwm(0xffff, 0xdff046);
  wbltapth((src >> 16) & 0x1f, 0xdff050);
  wbltaptl(src & 0xfffe, 0xdff052);
  wbltdpth((dst >> 16) & 0x1f, 0xdff054);
  wbltdptl(dst & 0xfffe, 0xdff056);

  // Width=1, height=4. BLTSIZE starts the blit when BLTEN is enabled.
  wbltsize((4u << 6) | 1u, 0xdff058);

  const bool started = blitterIsStarted();
  const uint32_t event_cycle = blitterEvent.cycle;
  if (event_cycle != BUS_CYCLE_DISABLE) {
    busRemoveEvent(&blitterEvent);
    bus.cycle = event_cycle;
    blitFinishBlit();
  }

  const uint16_t d0 = chipmemReadWord(dst + 0);
  const uint16_t d1 = chipmemReadWord(dst + 2);
  const uint16_t d2 = chipmemReadWord(dst + 4);
  const uint16_t d3 = chipmemReadWord(dst + 6);

  const bool pass =
      started &&
      d0 == 0x1111 && d1 == 0x2222 &&
      d2 == 0x3333 && d3 == 0x4444 &&
      !blitterIsStarted() &&
      !blitterGetDMAPending() &&
      !blitterGetZeroFlag();

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"blitter-a-to-d-copy-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"event_cycle\",\"value\":" << event_cycle << "},"
            << "{\"name\":\"d0\",\"value\":" << d0 << "},"
            << "{\"name\":\"d1\",\"value\":" << d1 << "},"
            << "{\"name\":\"d2\",\"value\":" << d2 << "},"
            << "{\"name\":\"d3\",\"value\":" << d3 << "}"
            << "]}\n";

  blitterShutdown();
  memoryShutdown();
  busShutdown();
  return pass ? 0 : 1;
}
