#pragma once

/*
 * Portable version metadata for the legacy WinFellow configuration core.
 *
 * The historical Windows build generated/provided versioninfo.h outside the
 * source tree.  Linux/SDL now compiles Configuration.cpp as part of the real
 * emulation core, so it needs the frontend-neutral description string too.
 * Keep this deliberately small: additional Windows resource metadata belongs
 * in the Windows build rather than in the portable core.
 */
#ifndef FELLOWLONGVERSION
#define FELLOWLONGVERSION "FellowNG 0.1.0"
#endif
