#pragma once

// Portable-host compatibility for legacy WinFellow CPU instruction code.
// The original profiling helpers use MSVC x86 inline assembly and are not
// valid with GCC/Clang. X64 already selects the existing non-asm path.
#if !defined(_WIN32)
#ifndef X64
#define X64 1
#endif

// A-line calltrap is an Amiga Forever/host integration hook. Portable Linux
// builds currently have no corresponding host service, so keep ordinary
// illegal-instruction handling buildable without pulling in Win32 glue.
static inline void call_calltrap(unsigned int)
{
}
#endif
