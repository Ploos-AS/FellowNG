#include <cstdint>
#include <iostream>

#include "Blitter.h"
#include "BusScheduler.h"
#include "CopperRegisters.h"
#include "LegacyCopper.h"
#include "MemoryInterface.h"
#include "GraphicsPipeline.h"
#include "graphics/Graphics.h"
#include "chipset.h"

namespace {
uint32_t gIoWrites = 0;
uint32_t gIoObserved = 0;

void captureIoWrite(uint16_t data, uint32_t)
{
  ++gIoWrites;
  gIoObserved = data;
}
} // namespace

int main()
{
  chipsetStartup();
  busStartup();
  busHardReset();
  memoryStartup();
  blitterStartup();
  copperStartup();
  graphStartup();

  constexpr uint32_t list = 0x1000;
  constexpr uint32_t source = 0x2000;

  chipmemWriteWord(0x0180, list + 0);
  chipmemWriteWord(0x0123, list + 2);
  chipmemWriteWord(0xffff, list + 4);
  chipmemWriteWord(0xfffe, list + 6);

  chipmemWriteWord(0xa55a, source);
  bpl1pt = source;
  bpl2pt = bpl3pt = bpl4pt = bpl5pt = bpl6pt = 0;
  _core.Registers.BplCon0 = 0x1000;
  oddscroll = evenscroll = 0;

  gIoWrites = 0;
  gIoObserved = 0;
  memorySetIoWriteStub(0x180, captureIoWrite);

  copper_registers.copper_dma = true;
  copper->Load(list);

  const uint32_t copper_cycle = copperEvent.cycle;
  const uint32_t pointer_before = bpl1pt;

  // Both operations are deliberately evaluated from the same deterministic
  // scheduler position. Copper owns its event slot first; bitplane DMA then
  // performs its fetch without modifying the Copper event queue.
  busRemoveEvent(&copperEvent);
  bus.cycle = copper_cycle;
  copperEvent.handler();

  const uint32_t after_copper_pc = copper_registers.copper_pc;
  const uint32_t after_copper_pointer = bpl1pt;

  GraphicsContext.Planar2ChunkyDecoder.NewBatch();
  GraphicsContext.BitplaneDMA.ProbeFetchLores();
  GraphicsContext.PixelSerializer.OutputCylindersUntil(0x1a, 88);

  const uint32_t after_dma_pointer = bpl1pt;
  const bool pass =
      copper_cycle == 4 &&
      gIoWrites == 1 &&
      gIoObserved == 0x0123 &&
      after_copper_pc == list + 4 &&
      after_copper_pointer == pointer_before &&
      after_dma_pointer == pointer_before + 2;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"copper-bitplane-contention-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"copper_cycle\",\"value\":" << copper_cycle << "},"
            << "{\"name\":\"copper_writes\",\"value\":" << writes << "},"
            << "{\"name\":\"copper_value\",\"value\":" << observed << "},"
            << "{\"name\":\"after_copper_pc\",\"value\":" << after_copper_pc << "},"
            << "{\"name\":\"after_dma_pointer\",\"value\":" << after_dma_pointer << "}"
            << "]}\n";
  return pass ? 0 : 1;
}
