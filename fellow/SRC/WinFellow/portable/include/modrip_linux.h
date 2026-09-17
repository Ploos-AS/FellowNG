#pragma once

/*
 * Portable host declarations for the legacy module-ripper UI hooks.
 *
 * The module-ripper core historically selected ModripWin32.h on Windows and
 * modrip_linux.h everywhere else, but the latter was never carried into the
 * WinFellow source tree.  Keep the portable core independent of the Windows
 * GUI by exposing the same host-hook contract here.  A portable frontend can
 * provide these hooks without pulling Win32 headers into the emulation core.
 */

#include "Defs.h"
#include "modrip.h"

extern BOOLE modripGuiInitialize();
extern void modripGuiSetBusy();
extern void modripGuiUnSetBusy();
extern BOOLE modripGuiSaveRequest(struct ModuleInfo *, MemoryAccessFunc);
extern void modripGuiErrorSave(struct ModuleInfo *);
extern BOOLE modripGuiRipFloppy(int);
extern BOOLE modripGuiRipMemory();
extern void modripGuiUnInitialize();
extern void modripGuiError(char *);
extern BOOLE modripGuiDumpChipMem();
extern BOOLE modripGuiRunProWiz();
