#include <cstdint>
#include <iostream>

#include "Blitter.h"
#include "BusScheduler.h"
#include "MemoryInterface.h"
#include "GraphicsPipeline.h"
#include "chipset.h"
#include "VirtualHost/CoreFactory.h"


int main() {
  std::cerr << "m9.3c: start\n";
  constexpr uint32_t src = 0x2000;
  constexpr uint32_t dst = 0x2100;

  CoreFactory::CreateServices();
  CoreFactory::CreateModules();
  chipsetStartup();
  busStartup();
  busHardReset();
  memoryStartup();
  blitterStartup();
  std::cerr << "m9.3c: startup complete\n";
  // Enable the legacy Blitter DMA gate consulted by wbltsize().
  dmacon = 0x0040;

  chipmemWriteWord(0x1111, src + 0);
  chipmemWriteWord(0x2222, src + 2);
  chipmemWriteWord(0x3333, src + 4);
  chipmemWriteWord(0x4444, src + 6);
  chipmemWriteWord(0, dst + 0);
  chipmemWriteWord(0, dst + 2);
  chipmemWriteWord(0, dst + 4);
  chipmemWriteWord(0, dst + 6);

  // A -> D, one word wide, four words high. Minterm 0xF0 selects A.
  std::cerr << "m9.3c: before registers\n";
  wbltcon0(0x09f0, 0xdff040);
  wbltcon1(0x0000, 0xdff042);
  wbltafwm(0xffff, 0xdff044);
  wbltalwm(0xffff, 0xdff046);
  wbltapth((src >> 16) & 0x1f, 0xdff050);
  wbltaptl(src & 0xfffe, 0xdff052);
  wbltdpth((dst >> 16) & 0x1f, 0xdff054);
  wbltdptl(dst & 0xfffe, 0xdff056);

  // Width=1, height=4. BLTSIZE starts the blit when BLTEN is enabled.
  std::cerr << "m9.3c: before BLTSIZE\n";
  wbltsize((4u << 6) | 1u, 0xdff058);
  std::cerr << "m9.3c: after BLTSIZE\n";

  const bool started_after_size = blitterIsStarted();
  const uint32_t scheduled_cycle = blitterEvent.cycle;
  const uint32_t pending_after_size = blitterGetDMAPending();
  const uint32_t event_cycle = blitterEvent.cycle;
  std::cerr << "m9.3c: before finish event=" << event_cycle << "\n";
  if (event_cycle != BUS_CYCLE_DISABLE) {
    bus.cycle = event_cycle;
    // busPopEvent() assumes another event follows it; this isolated probe has
    // only the Blitter event. Mirror the scheduler's ownership transition
    // without invoking that unsafe single-node helper.
    busRemoveEvent(&blitterEvent);
    blitterEvent.handler();
  }

  std::cerr << "m9.3c: before readback\n";
  const uint16_t d0 = chipmemReadWord(dst + 0);
  const uint16_t d1 = chipmemReadWord(dst + 2);
  const uint16_t d2 = chipmemReadWord(dst + 4);
  const uint16_t d3 = chipmemReadWord(dst + 6);

  const bool started = started_after_size;
  std::cerr << "m9.3c: observed started=" << started
            << " d0=" << std::hex << d0 << " d1=" << d1
            << " d2=" << d2 << " d3=" << d3
            << " pending=" << std::dec << blitterGetDMAPending()
            << " zero=" << blitterGetZeroFlag() << "\n";

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
            << "{\"name\":\"event_cycle\",\"value\":" << event_cycle << "},\n"
            << "{\"name\":\"scheduled_cycle\",\"value\":" << scheduled_cycle << "},\n"
            << "{\"name\":\"started_after_size\",\"value\":" << (started_after_size ? 1 : 0) << "},\n"
            << "{\"name\":\"dma_pending_after_size\",\"value\":" << pending_after_size << "},"
            << "{\"name\":\"d0\",\"value\":" << d0 << "},"
            << "{\"name\":\"d1\",\"value\":" << d1 << "},"
            << "{\"name\":\"d2\",\"value\":" << d2 << "},"
            << "{\"name\":\"d3\",\"value\":" << d3 << "}"
            << "]}\n";

  blitterShutdown();
  memoryShutdown();
  busShutdown();
  CoreFactory::DestroyModules();
  CoreFactory::DestroyServices();
  return pass ? 0 : 1;
}
