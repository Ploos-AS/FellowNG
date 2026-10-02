#include <cstdint>
#include <iostream>

#include "Defs.h"
#include "Defs.h"
#include "interrupt.h"
#include "VirtualHost/CoreFactory.h"

extern uint16_t intreq;
extern uint16_t intena;

static uint16_t pending_mask()
{
  return intena & intreq;
}

int main()
{
  CoreFactory::CreateServices();
  CoreFactory::CreateModules();
  interruptEmulationStart();
  std::cerr << "m9.3f: start\n";
  intena = 0;
  intreq = 0;
  std::cerr << "m9.3f: state reset\n";

  // Enable BLIT (bit 6) and request it through the real INTREQ write path.
  intena = 0x4000 | (1u << 6);
  std::cerr << "m9.3f: before INTREQ\n";
  wintreq_direct(0x8000 | (1u << 6), 0xdff09c, false);

  std::cerr << "m9.3f: after INTREQ\n";
  const uint16_t pending_after_set = pending_mask();
  const unsigned latency = interruptGetScheduleLatency();

  // Clear the request through the same register semantics.
  wintreq_direct(1u << 6, 0xdff09c, false);
  const uint16_t pending_after_clear = pending_mask();

  const bool pass =
      pending_after_set == (1u << 6) &&
      latency == 10 &&
      pending_after_clear == 0;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"interrupt-dma-arbitration-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"pending_after_set\",\"value\":" << pending_after_set << "},"
            << "{\"name\":\"schedule_latency\",\"value\":" << latency << "},"
            << "{\"name\":\"pending_after_clear\",\"value\":" << pending_after_clear << "}"
            << "]}\n";

  return pass ? 0 : 1;
}
