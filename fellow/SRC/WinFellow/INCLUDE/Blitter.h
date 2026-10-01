#pragma once

#include <cstdio>
#include <cstdint>
#include "Defs.h"

/*===========================================================================*/
/* Blitter-properties                                                        */
/*===========================================================================*/

extern void blitterSetFast(BOOLE fast);
extern BOOLE blitterGetFast();

extern void blitterSetOperationLog(BOOLE operation_log);
extern BOOLE blitterGetOperationLog();

extern BOOLE blitterGetDMAPending();
extern BOOLE blitterGetZeroFlag();
extern uint32_t blitterGetFreeCycles();
extern BOOLE blitterIsStarted();

/*===========================================================================*/
/* Declare C blitter functions                                               */
/*===========================================================================*/

extern void blitterSaveState(FILE *F);
extern void blitterLoadState(FILE *F);
void blitterEndOfFrame();
void blitterEmulationStart();
void blitterEmulationStop();
void blitterHardReset();
void blitterStartup();
void blitterShutdown();
void blitterLineMode();
void blitFinishBlit();
void blitForceFinish();
void blitterCopy();
// Register write handlers used by deterministic compatibility probes.
extern void blitterRemoveEvent();
extern void wbltcon0(uint16_t data, uint32_t address);
extern void wbltcon1(uint16_t data, uint32_t address);
extern void wbltafwm(uint16_t data, uint32_t address);
extern void wbltalwm(uint16_t data, uint32_t address);
extern void wbltapth(uint16_t data, uint32_t address);
extern void wbltaptl(uint16_t data, uint32_t address);
extern void wbltdpth(uint16_t data, uint32_t address);
extern void wbltdptl(uint16_t data, uint32_t address);
extern void wbltsize(uint16_t data, uint32_t address);

