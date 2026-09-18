/* Portable legacy input-driver lifecycle adapter.
 *
 * SDL/portable frontends feed input through IInputSource and
 * WinFellowRuntime::HandleInput. The common emulation modules still call the
 * historical host-driver lifecycle hooks, so keep those hooks state-neutral
 * on portable hosts instead of linking DirectInput/Win32 implementations.
 */
#include "Defs.h"

void kbdDrvHardReset() {}
void kbdDrvEmulationStart() {}
void kbdDrvEmulationStop() {}
void kbdDrvStartup() {}
void kbdDrvShutdown() {}

void mouseDrvHardReset() {}
BOOLE mouseDrvEmulationStart() { return TRUE; }
void mouseDrvEmulationStop() {}
void mouseDrvStartup() {}
void mouseDrvShutdown() {}

void joyDrvHardReset() {}
void joyDrvEmulationStart() {}
void joyDrvEmulationStop() {}
void joyDrvStartup() {}
void joyDrvShutdown() {}
