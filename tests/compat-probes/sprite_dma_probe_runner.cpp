#include <cstdint>
#include <iostream>

#include "chipset.h"
#include "MemoryInterface.h"
#include "SpriteRegisters.h"
#include "Sprites.h"
#include "Graphics.h"

int main()
{
  std::cerr << "m9.3e: before chipset\n";
  chipsetStartup();
  drawSetGraphicsEmulationMode(GRAPHICSEMULATIONMODE::GRAPHICSEMULATIONMODE_CYCLEEXACT);
  std::cerr << "m9.3e: after chipset\n";
  graphStartup();
  std::cerr << "m9.3e: after graph\n";
  spriteStartup();
  std::cerr << "m9.3e: after sprite\n";

  // Sprite 0: pointer, position/control, and two data words.
  std::cerr << "m9.3e: before pointer\n";
  wsprxpth(0x0000, 0xdff120);
  wsprxptl(0x2000, 0xdff122);
  std::cerr << "m9.3e: before pos\n";
  wsprxpos(0x2810, 0xdff140);
  std::cerr << "m9.3e: before ctl\n";
  wsprxctl(0x8301, 0xdff142);
  std::cerr << "m9.3e: before data\n";
  wsprxdata(0xf0f0, 0xdff144);
  std::cerr << "m9.3e: before datb\n";
  wsprxdatb(0x0f0f, 0xdff146);

  const uint32_t ptr = sprite_registers.sprpt[0];
  const uint16_t pos = sprite_registers.sprpos[0];
  const uint16_t ctl = sprite_registers.sprctl[0];
  const uint16_t data = sprite_registers.sprdata[0];
  const uint16_t datb = sprite_registers.sprdatb[0];

  // Pointer must remain word aligned; POS/CTL and data registers must retain
  // their documented control/data fields through the real IO handlers.
  const bool pass =
      ptr == 0x2000 &&
      pos == 0x2810 &&
      ctl == 0x8301 &&
      data == 0xf0f0 &&
      datb == 0x0f0f;

  std::cout << "{\"schema\":\"fellowng.compat-probe-result.v1\","
            << "\"probe\":\"sprite-register-dma-v1\","
            << "\"profile\":\"ocs-rom-free\","
            << "\"subsystem\":\"chipset\","
            << "\"status\":\"" << (pass ? "PASS" : "FAIL") << "\","
            << "\"observations\":["
            << "{\"name\":\"sprite0_pointer\",\"value\":" << ptr << "},"
            << "{\"name\":\"sprite0_pos\",\"value\":" << pos << "},"
            << "{\"name\":\"sprite0_ctl\",\"value\":" << ctl << "},"
            << "{\"name\":\"sprite0_data\",\"value\":" << data << "},"
            << "{\"name\":\"sprite0_datb\",\"value\":" << datb << "}"
            << "]}\n";

  spriteShutdown();
  graphShutdown();
  return pass ? 0 : 1;
}
