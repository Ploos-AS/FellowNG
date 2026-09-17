#pragma once

// WinFellow's system-information logger is implemented with Windows APIs.
// Keep the historical implementation on Windows, while allowing the portable
// emulator core to start without acquiring a Win32-only dependency.
#ifdef _WIN32
#include "../Windows/SystemInformation.h"
#else
inline void sysinfoLogSysInfo()
{
}
#endif
