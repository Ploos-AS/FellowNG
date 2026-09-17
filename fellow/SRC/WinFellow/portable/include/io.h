#pragma once

// Linux compatibility for legacy WinFellow sources that include the MSVC
// <io.h> header. POSIX exposes the corresponding low-level file operations
// through <unistd.h>. Keep this shim private to the portable build so the
// Windows toolchain continues to use its native <io.h>.
#include <limits.h>
#include <unistd.h>

// WinFellow's legacy sources use the Win32 MAX_PATH constant for stack
// buffers. Map it to the host path limit on portable builds.
#ifndef MAX_PATH
#ifdef PATH_MAX
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH 4096
#endif
#endif

// Windows headers historically supplied max as a macro to these C-era
// sources. Preserve that source contract for the portable C++ build without
// changing the native Windows build.
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
