#include <cstdint>
#include <iostream>

#include "chipset.h"
#include "GraphicsPipeline.h"
#include "MemoryInterface.h"
#include "graphics/Graphics.h"

int main()
{
  chipsetStartup();

  const uint32_t source = 0x2000;
  memory_chip[source] = 0xa5;
  memory_chip[source + 1] = 0x5a;

  bpl1pt = source;
  bpl2pt = bpl3pt = bpl4pt = bpl5pt = bpl6pt = 0;
  _core.Registers.BplCon0 = 0x1000; // one bitplane, lores
  oddscroll = evenscroll = 0;

  GraphicsContext.Planar2ChunkyDecoder.NewBatch();
  GraphicsContext.BitplaneDMA.ProbeFetchLores();

  const uint32_t pointer_after = bpl1pt;
  GraphicsContext.PixelSerializer.OutputCylindersUntil(0x1a, 72);
  const uint32_t batch_size = GraphicsContext.Planar2ChunkyDecoder.GetBatchSize();
  // The serializer emits the pre-fetch shift-register contents first. The
  // fetched word enters at scroll position 15, so its 16 decoded pixels begin
  // at batch offset 17 for the zero-scroll LORES case.
  GraphicsContext.PixelSerializer.OutputCylindersUntil(0x1a, 88);
  const uint32_t decoded_batch_size = GraphicsContext.Planar2ChunkyDecoder.GetBatchSize();
  const uint8_t *odd = GraphicsContext.Planar2ChunkyDecoder.GetOddPlayfield();

  uint32_t set_pixels = 0;
  for (uint32_t i = 17; i < decoded_batch_size; ++i)
  {
    if (odd[i] & 1u) ++set_pixels;
  }

  const bool pass = pointer_after == source + 2 && batch_size == 17 && decoded_batch_size >= 33 && set_pixels == 8;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"bitplane-lores-dma-fetch-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"source\",\"value\":" << source << "},"
            << "{\"name\":\"pointer_after\",\"value\":" << pointer_after << "},"
            << "{\"name\":\"batch_size_after_first_output\",\"value\":" << batch_size << "},"
            << "{\"name\":\"decoded_batch_size\",\"value\":" << decoded_batch_size << "},"
            << "{\"name\":\"set_pixels\",\"value\":" << set_pixels << "}"
            << "]}\n";
  return pass ? 0 : 1;
}
