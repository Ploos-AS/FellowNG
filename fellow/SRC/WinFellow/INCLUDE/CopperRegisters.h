#pragma once

#include "Defs.h"

class CopperRegisters
{
public:
  uint32_t copcon;
  uint32_t cop1lc;
  uint32_t cop2lc;
  uint32_t copper_pc;
  bool copper_dma;                /* Mirrors DMACON */
  uint32_t copper_suspended_wait; /* Position the copper should have been waiting for */
                                  /* if copper DMA had been turned on */

  void InstallIOHandlers();

  void ClearState();
  void LoadState(FILE *F);
  void SaveState(FILE *F);
};

extern CopperRegisters copper_registers;

// Custom-register write handlers. Kept public so deterministic chipset probes
// exercise the same register semantics installed in the memory I/O bank.
extern void wcopcon(uint16_t data, uint32_t address);
extern void wcop1lch(uint16_t data, uint32_t address);
extern void wcop1lcl(uint16_t data, uint32_t address);
extern void wcop2lch(uint16_t data, uint32_t address);
extern void wcop2lcl(uint16_t data, uint32_t address);
extern void wcopjmp1(uint16_t data, uint32_t address);
extern void wcopjmp2(uint16_t data, uint32_t address);
extern uint16_t rcopjmp1(uint32_t address);
extern uint16_t rcopjmp2(uint32_t address);
