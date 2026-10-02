#include <cstdint>
#include <iostream>

#include "SpriteP2CDecoder.h"

int main()
{
  SpriteP2CDecoder::Initialize();

  alignas(4) uint32_t pixels[4] = {};
  SpriteP2CDecoder::Decode4(0, pixels, 0xf0f0, 0x0f0f);

  uint32_t nonzero = 0;
  uint32_t plane0 = 0;
  uint32_t plane1 = 0;
  for (uint32_t word : pixels)
  {
    for (uint32_t shift = 0; shift < 32; shift += 8)
    {
      const uint8_t px = static_cast<uint8_t>((word >> shift) & 0xffu);
      if (px != 0) ++nonzero;
      if ((px & 0x04u) != 0) ++plane0;
      if ((px & 0x08u) != 0) ++plane1;
    }
  }

  const bool pass = nonzero == 16 && plane0 == 8 && plane1 == 8;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"sprite-p2c-decode-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"nonzero_pixels\",\"value\":" << nonzero << "},"
            << "{\"name\":\"plane0_pixels\",\"value\":" << plane0 << "},"
            << "{\"name\":\"plane1_pixels\",\"value\":" << plane1 << "}"
            << "]}\n";

  return pass ? 0 : 1;
}
