#pragma once

#include <cstdint>

// Pixel-renderer tables shared with Renderer.cpp and the graphics pipeline.
extern uint8_t draw_dual_translate[2][256][256];
extern uint32_t draw_HAM_modify_table[4][2];
extern const uint32_t draw_HAM_modify_table_bitindex;
extern const uint32_t draw_HAM_modify_table_holdmask;

void drawDualTranslationInitialize();
void drawHAMTableInit();
void drawWriteProfilingResultsToFile();
void drawModeFunctionsInitialize();
