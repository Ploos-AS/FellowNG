#pragma once

// Linux compatibility for legacy WinFellow sources that include the MSVC
// <io.h> header. POSIX exposes the corresponding low-level file operations
// through <unistd.h>. Keep this shim private to the portable build so the
// Windows toolchain continues to use its native <io.h>.
#include <unistd.h>
