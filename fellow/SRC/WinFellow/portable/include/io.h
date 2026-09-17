#pragma once

// Linux compatibility for legacy WinFellow sources that include the MSVC
// <io.h> header. POSIX exposes the corresponding low-level file operations
// through <unistd.h>. Keep this shim private to the portable build so the
// Windows toolchain continues to use its native <io.h>.
#include <limits.h>
#include <stdint.h>
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

// Some C-era WinFellow sources relied on the Win32 max macro. A function
// preserves those unqualified calls without a preprocessor macro that can
// corrupt C++ standard-library declarations such as std::max.
static inline uint32_t max(uint32_t a, uint32_t b)
{
  return a > b ? a : b;
}
