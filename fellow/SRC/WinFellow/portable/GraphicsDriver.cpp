/* Portable graphics driver backend for FellowNG.
 *
 * The renderer draws into a host-owned XRGB8888 framebuffer. Frame
 * presentation itself is frontend-neutral: GraphicsDriver.h calls
 * drawPresentCurrentFrame() before invalidating the buffer, and the portable
 * runtime forwards that frame to IVideoOutput (SDL3 today).
 */
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#include "GraphicsDriver.h"

namespace {
std::vector<uint8_t> framebuffer;
draw_buffer_information buffer{};
draw_mode current_mode{};

void configure(draw_mode *mode)
{
  if (mode != nullptr) current_mode = *mode;
  if (current_mode.width == 0) current_mode.width = 640;
  if (current_mode.height == 0) current_mode.height = 400;

  current_mode.bits = 32;
  const uint32_t pitch = current_mode.width * 4u;
  framebuffer.assign(static_cast<size_t>(pitch) * current_mode.height, 0);

  buffer.top_ptr = framebuffer.data();
  buffer.current_ptr = buffer.top_ptr;
  buffer.width = current_mode.width;
  buffer.height = current_mode.height;
  buffer.pitch = pitch;
  buffer.bits = 32;
  buffer.redsize = 8;
  buffer.redpos = 16;
  buffer.greensize = 8;
  buffer.greenpos = 8;
  buffer.bluesize = 8;
  buffer.bluepos = 0;
}
}

void gfxDrvClearCurrentBuffer()
{
  std::fill(framebuffer.begin(), framebuffer.end(), 0);
}

void gfxDrvBufferFlip()
{
  /* Presentation occurs in gfxDrvInvalidateBufferPointer(). */
}

void gfxDrvSetMode(draw_mode *mode, bool)
{
  configure(mode);
}

void gfxDrvSizeChanged(unsigned int width, unsigned int height)
{
  current_mode.width = width;
  current_mode.height = height;
  configure(&current_mode);
}

void gfxDrvPositionChanged() {}

uint8_t *gfxDrvValidateBufferPointer()
{
  if (framebuffer.empty()) configure(&current_mode);
  buffer.top_ptr = framebuffer.data();
  buffer.current_ptr = buffer.top_ptr;
  return buffer.top_ptr;
}

void gfxDrvInvalidateBufferPointerBackend()
{
  /* CPU framebuffer remains valid; the renderer may reuse it next frame. */
}

void gfxDrvGetBufferInformation(draw_buffer_information *information)
{
  if (information == nullptr) return;
  if (framebuffer.empty()) configure(&current_mode);
  *information = buffer;
}

uint32_t gfxDrvEmulationStartPost()
{
  return 1;
}

bool gfxDrvEmulationStart(unsigned int)
{
  if (framebuffer.empty()) configure(&current_mode);
  return true;
}

void gfxDrvEmulationStop() {}

void gfxDrvNotifyActiveStatus(bool) {}

DISPLAYDRIVER gfxDrvTryChangeDisplayDriver(DISPLAYDRIVER newDisplayDriver, bool)
{
  return newDisplayDriver;
}

bool gfxDrvStartup(DISPLAYDRIVER)
{
  current_mode.width = 640;
  current_mode.height = 400;
  current_mode.bits = 32;
  configure(&current_mode);
  return true;
}

void gfxDrvShutdown()
{
  framebuffer.clear();
  framebuffer.shrink_to_fit();
  buffer = {};
}

bool gfxDrvSaveScreenshot(const bool, const char *)
{
  /* Portable screenshot encoding is deliberately separate from presentation. */
  return false;
}

bool gfxDrvDXGIValidateRequirements()
{
  /* DXGI is a Windows-only backend and is not applicable to portable hosts. */
  return false;
}
