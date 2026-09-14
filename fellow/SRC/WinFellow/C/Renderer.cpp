/*=========================================================================*/
/* Fellow                                                                  */
/* Draws an Amiga screen in a host display buffer                          */
/*                                                                         */
/* Authors: Petter Schau                                                   */
/*          Worfje                                                         */
/*                                                                         */
/*                                                                         */
/* Copyright (C) 1991, 1992, 1996 Free Software Foundation, Inc.           */
/*                                                                         */
/* This program is free software; you can redistribute it and/or modify    */
/* it under the terms of the GNU General Public License as published by    */
/* the Free Software Foundation; either version 2, or (at your option)     */
/* any later version.                                                      */
/*                                                                         */
/* This program is distributed in the hope that it will be useful,         */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/* GNU General Public License for more details.                            */
/*                                                                         */
/* You should have received a copy of the GNU General Public License       */
/* along with this program; if not, write to the Free Software Foundation, */
/* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.          */
/*=========================================================================*/
#include <cmath>
#include <algorithm>

#include "Defs.h"
#include "FellowMain.h"
#include "BusScheduler.h"
#include "Renderer.h"
#include "MemoryInterface.h"
#include "GraphicsPipeline.h"
#include "GraphicsDriver.h"
#include "FellowList.h"
#include "Timers.h"
#include "FpsFont.h"
#include "LegacyCopper.h"
#include "Sprites.h"
#include "Configuration.h"

#include "draw_pixelrenderers.h"
#include "draw_interlace_control.h"

#include "graphics/Graphics.h"

#include <map>

#ifdef RETRO_PLATFORM
#include "RetroPlatform.h"
#endif

draw_mode_list draw_modes;
draw_mode *draw_mode_current;
draw_mode draw_mode_windowed;
draw_buffer_information draw_buffer_info;

namespace
{
  draw_frame_present_callback draw_frame_presenter = nullptr;
  void *draw_frame_presenter_context = nullptr;
}

void drawSetFramePresentCallback(draw_frame_present_callback callback, void *context)
{
  draw_frame_presenter = callback;
  draw_frame_presenter_context = context;
}

DISPLAYSCALE draw_displayscale;
DISPLAYSCALE_STRATEGY draw_displayscale_strategy;
DISPLAYDRIVER draw_displaydriver;
GRAPHICSEMULATIONMODE draw_graphicsemulationmode;

BOOLE draw_allow_multiple_buffers;
uint32_t draw_clear_buffers;

draw_rect draw_buffer_clip;

void drawSetBufferClip(const draw_rect &buffer_clip)
{
  draw_buffer_clip = buffer_clip;
}

const draw_rect &drawGetBufferClip()
{
  return draw_buffer_clip;
}

uint32_t drawGetBufferClipLeft()
{
  return draw_buffer_clip.left;
}

float drawGetBufferClipLeftAsFloat()
{
  return static_cast<float>(drawGetBufferClipLeft());
}

uint32_t drawGetBufferClipTop()
{
  return draw_buffer_clip.top;
}

float drawGetBufferClipTopAsFloat()
{
  return static_cast<float>(drawGetBufferClipTop());
}

uint32_t drawGetBufferClipWidth()
{
  return draw_buffer_clip.GetWidth();
}

float drawGetBufferClipWidthAsFloat()
{
  return static_cast<float>(drawGetBufferClipWidth());
}

uint32_t drawGetBufferClipHeight()
{
  return draw_buffer_clip.GetHeight();
}

float drawGetBufferClipHeightAsFloat()
{
  return static_cast<float>(drawGetBufferClipHeight());
}

draw_rect draw_clip_max_pal;
draw_rect draw_internal_clip;
draw_rect draw_output_clip;

void drawSetInternalClip(const draw_rect &internal_clip)
{
#ifdef _DEBUG
  _core.Log->AddLog("drawSetInternalClip(rectangle left=%d, top=%d, right=%d, bottom=%d)\n", internal_clip.left, internal_clip.top, internal_clip.right, internal_clip.bottom);
#endif
  draw_internal_clip = internal_clip;
}

const draw_rect &drawGetInternalClip()
{
  return draw_internal_clip;
}

void drawSetOutputClip(const draw_rect &output_clip)
{
#ifdef _DEBUG
  _core.Log->AddLog("drawSetOutputClip(rectangle left=%d, top=%d, right=%d, bottom=%d)\n", output_clip.left, output_clip.top, output_clip.right, output_clip.bottom);
#endif
  draw_output_clip = output_clip;
}

const draw_rect &drawGetOutputClip()
{
  return draw_output_clip;
}

void drawInitializePredefinedClipRectangles()
{
  draw_clip_max_pal.left = 88;
  draw_clip_max_pal.top = 26;
  draw_clip_max_pal.right = 472;
  draw_clip_max_pal.bottom = 314;
}

void drawAddMode(draw_mode *modenode)
{
  draw_modes.push_back(modenode);
}

static draw_mode *drawGetFirstMode()
{
  return (draw_modes.empty()) ? nullptr : draw_modes.front();
}

void drawClearModeList()
{
  for (draw_mode *dm : draw_modes)
  {
    delete dm;
  }
  draw_modes.clear();
  draw_mode_current = &draw_mode_windowed;
}

static draw_mode *drawFindMode(uint32_t width, uint32_t height, uint32_t colorbits, uint32_t refresh, bool allow_any_refresh)
{
  auto item_iterator = std::find_if(draw_modes.begin(), draw_modes.end(), [width, height, colorbits, refresh, allow_any_refresh](draw_mode *dm) {
    return (dm->width == width) && (dm->height == height) && (dm->bits == colorbits) && (allow_any_refresh || (dm->refresh == refresh));
  });

  return (item_iterator != draw_modes.end()) ? *item_iterator : nullptr;
}

draw_mode_list &drawGetModes()
{
  return draw_modes;
}

uint32_t draw_color_table[4096];

draw_line_func draw_line_routine;
draw_line_func draw_line_BG_routine;
draw_line_func draw_line_BPL_manage_routine;
draw_line_func draw_line_BPL_res_routine;
draw_line_func draw_line_lores_routine;
draw_line_func draw_line_hires_routine;
draw_line_func draw_line_dual_lores_routine;
draw_line_func draw_line_dual_hires_routine;
draw_line_func draw_line_HAM_lores_routine;

uint32_t draw_buffer_show;
uint32_t draw_buffer_draw;
uint32_t draw_buffer_count;
uint32_t draw_frame_count;
uint32_t draw_frame_skip_factor;
int32_t draw_frame_skip;
uint32_t draw_switch_bg_to_bpl;

#define DRAW_LED_COUNT 5
#define DRAW_LED_WIDTH 12
#define DRAW_LED_HEIGHT 4
#define DRAW_LED_FIRST_X 16
#define DRAW_LED_FIRST_Y 4
#define DRAW_LED_GAP 8
#define DRAW_LED_COLOR_ON 0x00FF00
#define DRAW_LED_COLOR_OFF 0x000000

bool draw_LEDs_enabled;
bool draw_LEDs_state[DRAW_LED_COUNT];

static void drawLED16(int x, int y, int width, int height, uint32_t color)
{
  uint16_t *bufw = ((uint16_t *)(draw_buffer_info.top_ptr + draw_buffer_info.pitch * y)) + x;
  uint16_t color16 = (uint16_t)draw_color_table[((color & 0xf00000) >> 12) | ((color & 0x00f000) >> 8) | ((color & 0x0000f0) >> 4)];
  for (int y1 = 0; y1 < height; y1++)
  {
    for (int x1 = 0; x1 < width; x1++) *(bufw + x1) = color16;
    bufw = (uint16_t *)(((uint8_t *)bufw) + draw_buffer_info.pitch);
  }
}

static void drawLED24(int x, int y, int width, int height, uint32_t color)
{
  uint8_t *bufb = draw_buffer_info.top_ptr + draw_buffer_info.pitch * y + x * 3;
  uint32_t color24 = draw_color_table[((color & 0xf00000) >> 12) | ((color & 0x00f000) >> 8) | ((color & 0x0000f0) >> 4)];
  uint8_t color24_1 = (uint8_t)((color24 & 0xff0000) >> 16);
  uint8_t color24_2 = (uint8_t)((color24 & 0x00ff00) >> 8);
  uint8_t color24_3 = (uint8_t)(color24 & 0x0000ff);
  for (int y1 = 0; y1 < height; y1++)
  {
    for (int x1 = 0; x1 < width; x1++)
    {
      *(bufb + x1 * 3) = color24_1;
      *(bufb + x1 * 3 + 1) = color24_2;
      *(bufb + x1 * 3 + 2) = color24_3;
    }
    bufb = bufb + draw_buffer_info.pitch;
  }
}

static void drawLED32(int x, int y, int width, int height, uint32_t color)
{
  uint32_t *bufl = ((uint32_t *)(draw_buffer_info.top_ptr + draw_buffer_info.pitch * y)) + x;
  uint32_t color32 = draw_color_table[((color & 0xf00000) >> 12) | ((color & 0x00f000) >> 8) | ((color & 0x0000f0) >> 4)];
  for (int y1 = 0; y1 < height; y1++)
  {
    for (int x1 = 0; x1 < width; x1++) *(bufl + x1) = color32;
    bufl = (uint32_t *)(((uint8_t *)bufl) + draw_buffer_info.pitch);
  }
}

static void drawLED(int index, bool state)
{
  int x, y, height, width;
#ifdef RETRO_PLATFORM
  if (!RP.GetHeadlessMode())
  {
#endif
    x = DRAW_LED_FIRST_X + (DRAW_LED_WIDTH + DRAW_LED_GAP) * index + (drawGetOutputClip().left - drawGetInternalClip().left) * drawGetOutputScaleFactor();
    y = DRAW_LED_FIRST_Y + (drawGetOutputClip().top - drawGetInternalClip().top) * drawGetOutputScaleFactor();
    height = DRAW_LED_HEIGHT;
    width = DRAW_LED_WIDTH;
#ifdef RETRO_PLATFORM
  }
  else
  {
    x = DRAW_LED_FIRST_X + (DRAW_LED_WIDTH + DRAW_LED_GAP) * index + (drawGetOutputClip().left / 2 - drawGetInternalClip().left) * 2;
    y = DRAW_LED_FIRST_Y + (drawGetOutputClip().top / 2 - drawGetInternalClip().top) * 2;
    height = DRAW_LED_HEIGHT / 2;
    width = DRAW_LED_WIDTH / 2;
  }
#endif
  uint32_t color = (state) ? DRAW_LED_COLOR_ON : DRAW_LED_COLOR_OFF;
  switch (draw_buffer_info.bits)
  {
    case 16: drawLED16(x, y, DRAW_LED_WIDTH, height, color); break;
    case 24: drawLED24(x, y, DRAW_LED_WIDTH, height, color); break;
    case 32: drawLED32(x, y, DRAW_LED_WIDTH, height, color); break;
    default: break;
  }
}

static void drawLEDs()
{
  if (draw_LEDs_enabled)
  {
    for (int i = 0; i < DRAW_LED_COUNT; i++) drawLED(i, draw_LEDs_state[i]);
  }
}

bool draw_fps_counter_enabled;
bool draw_fps_buffer[5][20];

static void drawFpsChar(int character, int x)
{
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 4; j++) draw_fps_buffer[i][x * 4 + j] = draw_fps_font[character][i][j];
}

static void drawFpsText(char *text)
{
  for (int i = 0; i < 4; i++)
  {
    char c = *text++;
    switch (c)
    {
      case '0': drawFpsChar(9, i); break;
      case '1': drawFpsChar(0, i); break;
      case '2': drawFpsChar(1, i); break;
      case '3': drawFpsChar(2, i); break;
      case '4': drawFpsChar(3, i); break;
      case '5': drawFpsChar(4, i); break;
      case '6': drawFpsChar(5, i); break;
      case '7': drawFpsChar(6, i); break;
      case '8': drawFpsChar(7, i); break;
      case '9': drawFpsChar(8, i); break;
      case '%': drawFpsChar(10, i); break;
      case ' ': default: drawFpsChar(11, i); break;
    }
  }
}

static void drawFpsToFramebuffer16()
{
  uint16_t *bufw = ((uint16_t *)draw_buffer_info.top_ptr) + draw_buffer_info.width - 20;
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 20; x++) *(bufw + x) = draw_fps_buffer[y][x] ? 0xffff : 0;
    bufw = (uint16_t *)(((uint8_t *)bufw) + draw_buffer_info.pitch);
  }
}

static void drawFpsToFramebuffer24()
{
  uint8_t *bufb = draw_buffer_info.top_ptr + (draw_buffer_info.width - 20) * 3;
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 20; x++)
    {
      uint8_t color = draw_fps_buffer[y][x] ? 0xff : 0;
      *(bufb + x * 3) = color;
      *(bufb + x * 3 + 1) = color;
      *(bufb + x * 3 + 2) = color;
    }
    bufb += draw_buffer_info.pitch;
  }
}

static void drawFpsToFramebuffer32()
{
  uint32_t *bufl = ((uint32_t *)draw_buffer_info.top_ptr) + draw_buffer_info.width - 20;
#ifdef RETRO_PLATFORM
  if (RP.GetHeadlessMode())
  {
    bufl -= RETRO_PLATFORM_MAX_PAL_LORES_WIDTH * 2 - RP.GetScreenWidthAdjusted() / RP.GetDisplayScale() - RP.GetClippingOffsetLeftAdjusted();
    bufl += RP.GetClippingOffsetTopAdjusted() * draw_buffer_info.pitch / 4;
  }
#endif
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 20; x++) *(bufl + x) = draw_fps_buffer[y][x] ? 0xffffffff : 0;
    bufl = (uint32_t *)(((uint8_t *)bufl) + draw_buffer_info.pitch);
  }
}

static void drawFpsCounter()
{
  if (draw_fps_counter_enabled)
  {
    char s[16];
    sprintf(s, "%u", drawStatLast50FramesFps());
    drawFpsText(s);
    switch (draw_buffer_info.bits)
    {
      case 16: drawFpsToFramebuffer16(); break;
      case 24: drawFpsToFramebuffer24(); break;
      case 32: drawFpsToFramebuffer32(); break;
      default: break;
    }
  }
}

void drawSetFullScreenMode(uint32_t width, uint32_t height, uint32_t colorbits, uint32_t refresh)
{
#ifdef RETRO_PLATFORM
  if (RP.GetHeadlessMode())
  {
    height = RETRO_PLATFORM_MAX_PAL_LORES_HEIGHT * 2;
    width = RETRO_PLATFORM_MAX_PAL_LORES_WIDTH * 2;
  }
#endif
  draw_mode *mode_found = drawFindMode(width, height, colorbits, refresh, false);
  if (mode_found == nullptr) mode_found = drawFindMode(width, height, colorbits, refresh, true);
  draw_mode_current = (mode_found != nullptr) ? mode_found : drawGetFirstMode();
  gfxDrvGetBufferInformation(&draw_buffer_info);
}

void drawSetWindowedMode(uint32_t width, uint32_t height)
{
  draw_mode_windowed.width = width;
  draw_mode_windowed.height = height;
  draw_mode_windowed.bits = 32;
  draw_mode_windowed.refresh = 0;
  draw_mode_current = &draw_mode_windowed;
  gfxDrvGetBufferInformation(&draw_buffer_info);
}

void drawSetDisplayScale(DISPLAYSCALE displayscale) { draw_displayscale = displayscale; }
DISPLAYSCALE drawGetDisplayScale() { return draw_displayscale; }

static uint32_t drawGetAutomaticInternalScaleFactor() { return (draw_mode_current->width < 1280) ? 2 : 4; }
uint32_t drawGetInternalScaleFactor()
{
  if (drawGetDisplayScale() == DISPLAYSCALE::DISPLAYSCALE_AUTO) return drawGetAutomaticInternalScaleFactor();
  return (draw_displayscale == DISPLAYSCALE::DISPLAYSCALE_1X) ? 2 : 4;
}

uint32_t drawGetOutputScaleFactor()
{
  if (RP.GetHeadlessMode()) return RP.GetDisplayScale() * 2;
  uint32_t output_scale_factor = 2;
  switch (drawGetDisplayScale())
  {
    case DISPLAYSCALE::DISPLAYSCALE_1X: output_scale_factor = 2; break;
    case DISPLAYSCALE::DISPLAYSCALE_2X: output_scale_factor = 4; break;
    case DISPLAYSCALE::DISPLAYSCALE_3X: output_scale_factor = 6; break;
    case DISPLAYSCALE::DISPLAYSCALE_4X: output_scale_factor = 8; break;
  }
  return output_scale_factor;
}

void drawSetDisplayScaleStrategy(DISPLAYSCALE_STRATEGY displayscalestrategy) { draw_displayscale_strategy = displayscalestrategy; }
DISPLAYSCALE_STRATEGY drawGetDisplayScaleStrategy() { return draw_displayscale_strategy; }
void drawSetDisplayDriver(DISPLAYDRIVER displaydriver) { draw_displaydriver = displaydriver; }
DISPLAYDRIVER drawGetDisplayDriver() { return draw_displaydriver; }

void drawSetGraphicsEmulationMode(GRAPHICSEMULATIONMODE graphicsemulationmode)
{
  GRAPHICSEMULATIONMODE oldgraphicsemulationmode = draw_graphicsemulationmode;
  draw_graphicsemulationmode = graphicsemulationmode;
  if (oldgraphicsemulationmode != draw_graphicsemulationmode)
  {
    spriteInitializeFromEmulationMode();
    copperInitializeFromEmulationMode();
  }
}

GRAPHICSEMULATIONMODE drawGetGraphicsEmulationMode() { return draw_graphicsemulationmode; }
void drawSetFrameskipRatio(uint32_t frameskipratio) { draw_frame_skip_factor = frameskipratio; }
void drawSetFPSCounterEnabled(bool enabled) { draw_fps_counter_enabled = enabled; }
void drawSetLEDsEnabled(bool enabled) { draw_LEDs_enabled = enabled; }
void drawSetLED(int index, bool state) { if (index < DRAW_LED_COUNT) draw_LEDs_state[index] = state; }
void drawSetAllowMultipleBuffers(BOOLE allow_multiple_buffers) { draw_allow_multiple_buffers = allow_multiple_buffers; }
BOOLE drawGetAllowMultipleBuffers() { return draw_allow_multiple_buffers; }
uint32_t drawGetBufferCount() { return draw_buffer_count; }

static void drawColorTranslationInitialize()
{
  for (uint32_t k = 0; k < 4096; k++)
  {
    uint32_t r = ((k & 0xf00) >> 8) << (draw_buffer_info.redpos + draw_buffer_info.redsize - 4);
    uint32_t g = ((k & 0xf0) >> 4) << (draw_buffer_info.greenpos + draw_buffer_info.greensize - 4);
    uint32_t b = (k & 0xf) << (draw_buffer_info.bluepos + draw_buffer_info.bluesize - 4);
    draw_color_table[k] = r | g | b;
    if (draw_buffer_info.bits <= 16) draw_color_table[k] = draw_color_table[k] << 16 | draw_color_table[k];
  }
}

std::pair<uint32_t, uint32_t> drawCalculateHorizontalOutputClip(uint32_t buffer_width, uint32_t buffer_scale_factor)
{
  uint32_t left, right;
  if (!RP.GetHeadlessMode() && drawGetDisplayScale() != DISPLAYSCALE::DISPLAYSCALE_AUTO)
  {
    uint32_t width_amiga = buffer_width / buffer_scale_factor;
    const draw_rect &internal_clip = drawGetInternalClip();
    if (width_amiga > internal_clip.GetWidth()) width_amiga = internal_clip.GetWidth();
    if (width_amiga <= 343)
    {
      left = 129;
      if (left < internal_clip.left || left >= internal_clip.right || (left + width_amiga) > internal_clip.right) left = internal_clip.left;
      right = left + width_amiga;
    }
    else
    {
      right = internal_clip.right;
      left = right - width_amiga;
    }
  }
  else
  {
    left = drawGetOutputClip().left;
    right = drawGetOutputClip().right;
  }
  return {left, right};
}

std::pair<uint32_t, uint32_t> drawCalculateVerticalOutputClip(uint32_t buffer_height, uint32_t buffer_scale_factor)
{
  uint32_t top, bottom;
  if (!RP.GetHeadlessMode() && drawGetDisplayScale() != DISPLAYSCALE::DISPLAYSCALE_AUTO)
  {
    uint32_t height_amiga = buffer_height / buffer_scale_factor;
    const draw_rect &internal_clip = drawGetInternalClip();
    if (height_amiga > internal_clip.GetHeight()) height_amiga = internal_clip.GetHeight();
    if (height_amiga <= 270)
    {
      top = 44;
      if (top < internal_clip.top || top >= internal_clip.bottom || (top + height_amiga) > internal_clip.bottom) top = internal_clip.top;
      bottom = top + height_amiga;
    }
    else
    {
      bottom = internal_clip.bottom;
      top = bottom - height_amiga;
    }
  }
  else
  {
    top = drawGetOutputClip().top;
    bottom = drawGetOutputClip().bottom;
  }
  return {top, bottom};
}

static void drawAmigaScreenGeometry(uint32_t buffer_width, uint32_t buffer_height)
{
  uint32_t output_scale_factor = drawGetOutputScaleFactor();
  uint32_t internal_scale_factor = drawGetInternalScaleFactor();
  uint32_t buffer_clip_left, buffer_clip_top, buffer_clip_width, buffer_clip_height;
  const auto horizontal_clip = drawCalculateHorizontalOutputClip(draw_mode_current->width, output_scale_factor);
  const auto vertical_clip = drawCalculateVerticalOutputClip(draw_mode_current->height, output_scale_factor);
  draw_rect output_clip(horizontal_clip.first, vertical_clip.first, horizontal_clip.second, vertical_clip.second);
  drawSetOutputClip(output_clip);
  const draw_rect &internal_clip = drawGetInternalClip();
  if (!RP.GetHeadlessMode())
  {
    buffer_clip_left = (output_clip.left - internal_clip.left) * internal_scale_factor;
    buffer_clip_top = (output_clip.top - internal_clip.top) * internal_scale_factor;
    buffer_clip_width = output_clip.GetWidth() * internal_scale_factor;
    buffer_clip_height = output_clip.GetHeight() * internal_scale_factor;
  }
  else
  {
    buffer_clip_left = output_clip.left - (internal_clip.left * internal_scale_factor);
    buffer_clip_top = output_clip.top - (internal_clip.top * internal_scale_factor);
    buffer_clip_width = output_clip.GetWidth();
    buffer_clip_height = output_clip.GetHeight();
  }
  drawSetBufferClip(draw_rect(buffer_clip_left, buffer_clip_top, buffer_clip_left + buffer_clip_width, buffer_clip_top + buffer_clip_height));
}

static void drawModeTablesInitialize()
{
  draw_buffer_draw = 0;
  draw_buffer_show = 0;
  drawModeFunctionsInitialize();
  wbplcon0((uint16_t)_core.Registers.BplCon0, 0xdff100);
  drawColorTranslationInitialize();
  graphInitializeShadowColors();
  draw_buffer_info.current_ptr = draw_buffer_info.top_ptr;
  drawHAMTableInit();
}

static void drawBufferFlip()
{
  if (++draw_buffer_show >= draw_buffer_count) draw_buffer_show = 0;
  if (++draw_buffer_draw >= draw_buffer_count) draw_buffer_draw = 0;
  gfxDrvBufferFlip();
}

uint32_t draw_stat_first_frame_timestamp;
uint32_t draw_stat_last_frame_timestamp;
uint32_t draw_stat_last_50_timestamp;
uint32_t draw_stat_last_frame_ms;
uint32_t draw_stat_last_50_ms;
uint32_t draw_stat_frame_count;

void drawStatClear()
{
  draw_stat_last_50_ms = 0;
  draw_stat_last_frame_ms = 0;
  draw_stat_frame_count = 0;
}

void drawStatTimestamp()
{
  uint32_t timestamp = timerGetTimeMs();
  if (draw_stat_frame_count == 0)
  {
    draw_stat_first_frame_timestamp = timerGetTimeMs();
    draw_stat_last_frame_timestamp = draw_stat_first_frame_timestamp;
    draw_stat_last_50_timestamp = draw_stat_first_frame_timestamp;
  }
  else
  {
    draw_stat_last_frame_ms = timestamp - draw_stat_last_frame_timestamp;
    draw_stat_last_frame_timestamp = timestamp;
    if ((draw_stat_frame_count % 50) == 0)
    {
      draw_stat_last_50_ms = timestamp - draw_stat_last_50_timestamp;
      draw_stat_last_50_timestamp = timestamp;
    }
  }
  draw_stat_frame_count++;
}

uint32_t drawStatLast50FramesFps() { return draw_stat_last_50_ms == 0 ? 0 : 50000 / draw_stat_last_50_ms; }
uint32_t drawStatLastFrameFps() { return draw_stat_last_frame_ms == 0 ? 0 : 1000 / draw_stat_last_frame_ms; }
uint32_t drawStatSessionFps()
{
  uint32_t session_time = draw_stat_last_frame_timestamp - draw_stat_first_frame_timestamp;
  return session_time == 0 ? 0 : (draw_frame_count * 20) / (session_time + 14);
}

uint32_t drawValidateBufferPointer(uint32_t amiga_line_number)
{
  uint32_t internal_scale_factor = drawGetInternalScaleFactor();
  draw_buffer_info.top_ptr = gfxDrvValidateBufferPointer();
  if (draw_buffer_info.top_ptr == nullptr)
  {
    _core.Log->AddLog("Buffer ptr is nullptr\n");
    return 0;
  }
  draw_buffer_info.current_ptr = draw_buffer_info.top_ptr + draw_buffer_info.pitch * internal_scale_factor * (amiga_line_number - drawGetInternalClip().top);
  if (drawGetUseInterlacedRendering() && !drawGetFrameIsLong())
    draw_buffer_info.current_ptr += (draw_buffer_info.pitch * internal_scale_factor) / 2;
  return draw_buffer_info.pitch * internal_scale_factor;
}

void drawInvalidateBufferPointer() { gfxDrvInvalidateBufferPointer(); }
void drawHardReset() { draw_switch_bg_to_bpl = FALSE; }

bool drawEmulationStart()
{
  uint32_t gfxModeNumberOfBuffers = (drawGetAllowMultipleBuffers() && !drawGetDeinterlace()) ? 3 : 1;
  draw_switch_bg_to_bpl = FALSE;
  draw_frame_skip = 0;
  gfxDrvSetMode(draw_mode_current, draw_mode_current == &draw_mode_windowed);
  if (!gfxDrvEmulationStart(gfxModeNumberOfBuffers))
  {
    fellowShowRequester(FELLOW_REQUESTER_TYPE::FELLOW_REQUESTER_TYPE_ERROR, "Failure: The graphics driver failed to start. See fellow.log for more details.");
    return false;
  }
  drawStatClear();
  return true;
}

bool drawEmulationStartPost()
{
  drawAmigaScreenGeometry(draw_buffer_info.width, draw_buffer_info.height);
  draw_buffer_count = gfxDrvEmulationStartPost();
  if (draw_buffer_count == 0)
  {
    fellowShowRequester(FELLOW_REQUESTER_TYPE::FELLOW_REQUESTER_TYPE_ERROR, "Failure: The graphics driver failed to start. See fellow.log for more details.");
    return false;
  }
  draw_buffer_show = 0;
  draw_buffer_draw = draw_buffer_count - 1;
  drawModeTablesInitialize();
  _core.Log->AddLog("drawEmulationStartPost(): Buffer is (%d,%d,%d)\n", draw_buffer_info.width, draw_buffer_info.height, draw_buffer_info.bits);
  return true;
}

void drawEmulationStop() { gfxDrvEmulationStop(); }

BOOLE drawStartup()
{
  cfg *initialConfig = cfgManagerGetCurrentConfig(&cfg_manager);
  drawClearModeList();
  if (!gfxDrvStartup(cfgGetDisplayDriver(initialConfig))) return FALSE;
  draw_mode_windowed.width = 640;
  draw_mode_windowed.height = 400;
  draw_mode_windowed.bits = 32;
  draw_mode_current = &draw_mode_windowed;
  drawDualTranslationInitialize();
  drawInitializePredefinedClipRectangles();
  if (!RP.GetHeadlessMode())
  {
    drawSetInternalClip(draw_clip_max_pal);
    drawSetOutputClip(draw_clip_max_pal);
  }
  draw_switch_bg_to_bpl = FALSE;
  draw_frame_count = 0;
  draw_clear_buffers = 0;
  drawSetDisplayScale(DISPLAYSCALE::DISPLAYSCALE_1X);
  drawSetDisplayScaleStrategy(DISPLAYSCALE_STRATEGY::DISPLAYSCALE_STRATEGY_SOLID);
  drawSetFrameskipRatio(1);
  drawSetFPSCounterEnabled(false);
  drawSetLEDsEnabled(false);
  drawSetAllowMultipleBuffers(FALSE);
  drawSetGraphicsEmulationMode(cfgGetGraphicsEmulationMode(initialConfig));
  drawInterlaceStartup();
  for (uint32_t i = 0; i < DRAW_LED_COUNT; i++) drawSetLED(i, false);
  return TRUE;
}

void drawShutdown()
{
  drawClearModeList();
  gfxDrvShutdown();
}

void drawUpdateDrawmode()
{
  draw_line_routine = draw_line_BG_routine;
  if (graph_playfield_on == 1 && ((dmacon & 0x0100) == 0x0100) && _core.RegisterUtility.GetEnabledBitplaneCount() != 0)
  {
    draw_switch_bg_to_bpl = TRUE;
    draw_line_routine = draw_line_BPL_manage_routine;
  }
}

uint32_t drawGetNextLineOffsetInBytes(uint32_t pitch_in_bytes)
{
  uint32_t internal_scale_factor = drawGetInternalScaleFactor();
  if (internal_scale_factor == 2 && drawGetDisplayScaleStrategy() == DISPLAYSCALE_STRATEGY::DISPLAYSCALE_STRATEGY_SCANLINES) return 0;
  if (internal_scale_factor == 2 && drawGetDisplayScaleStrategy() == DISPLAYSCALE_STRATEGY::DISPLAYSCALE_STRATEGY_SOLID) return pitch_in_bytes / 2;
  if (internal_scale_factor == 4 && drawGetDisplayScaleStrategy() == DISPLAYSCALE_STRATEGY::DISPLAYSCALE_STRATEGY_SCANLINES) return pitch_in_bytes / 4;
  return pitch_in_bytes / 4;
}

void drawReinitializeRendering()
{
  drawModeTablesInitialize();
  graphLineDescClear();
}

void drawEndOfFrame()
{
  if (draw_frame_skip == 0)
  {
    if (draw_clear_buffers > 0)
    {
      gfxDrvClearCurrentBuffer();
      --draw_clear_buffers;
    }

    uint32_t pitch_in_bytes = drawValidateBufferPointer(drawGetInternalClip().top);
    if (draw_buffer_info.top_ptr != nullptr)
    {
      if (drawGetGraphicsEmulationMode() == GRAPHICSEMULATIONMODE::GRAPHICSEMULATIONMODE_LINEEXACT)
      {
        uint32_t height = drawGetInternalClip().GetHeight();
        uint8_t *draw_buffer_current_ptr_local = draw_buffer_info.current_ptr;
        for (uint32_t i = 0; i < height; i++)
        {
          graph_line *graph_frame_ptr = graphGetLineDesc(draw_buffer_draw, drawGetInternalClip().top + i);
          if (graph_frame_ptr != nullptr && graph_frame_ptr->linetype != graph_linetypes::GRAPH_LINE_SKIP && graph_frame_ptr->linetype != graph_linetypes::GRAPH_LINE_BPL_SKIP)
            ((draw_line_func)(graph_frame_ptr->draw_line_routine))(graph_frame_ptr, drawGetNextLineOffsetInBytes(pitch_in_bytes));
          draw_buffer_current_ptr_local += pitch_in_bytes;
          draw_buffer_info.current_ptr = draw_buffer_current_ptr_local;
        }
      }
      else
      {
        GraphicsContext.BitplaneDraw.TmpFrame(pitch_in_bytes);
      }

      drawLEDs();
      drawFpsCounter();
      if (draw_frame_presenter != nullptr)
      {
        draw_frame_presenter(draw_buffer_info, draw_frame_presenter_context);
      }
      drawInvalidateBufferPointer();
      drawStatTimestamp();
      drawBufferFlip();
    }
  }

  draw_frame_count++;
  draw_frame_skip--;
  if (draw_frame_skip < 0) draw_frame_skip = draw_frame_skip_factor;
}
