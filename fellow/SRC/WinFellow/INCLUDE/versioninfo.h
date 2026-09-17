#pragma once

/*
 * Portable version metadata for the legacy WinFellow configuration core.
 *
 * The historical Windows build generated/provided versioninfo.h outside the
 * source tree. Linux/SDL now compiles Configuration.cpp and FellowMain.cpp as
 * part of the real emulation core, so keep the frontend-neutral version names
 * available here. Windows resource metadata remains a Windows build concern.
 */
#ifndef FELLOWVERSION
#define FELLOWVERSION "FellowNG 0.1.0"
#endif

#ifndef FELLOWLONGVERSION
#define FELLOWLONGVERSION FELLOWVERSION
#endif

/*
 * FellowMain.cpp still contains a few MSVC spellings. Keep the compatibility
 * local to the legacy version header while the portable runtime is brought up.
 */
#if !defined(_WIN32)
#include <cstdarg>
#include <cstdio>
#ifndef __cdecl
#define __cdecl
#endif
#ifndef _vsnprintf
#define _vsnprintf std::vsnprintf
#endif
#endif
