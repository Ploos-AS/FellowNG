#pragma once

// Legacy WinFellow sources still include RetroPlatform.h from the common
// include directory. The native Windows build still provides the historical
// RetroPlatform implementation and expects its RP singleton/API declarations.
// Portable builds deliberately keep that dependency out of the core.
#if defined(_WIN32) && defined(RETRO_PLATFORM)
#include "../Windows/RetroPlatform.h"
#endif
