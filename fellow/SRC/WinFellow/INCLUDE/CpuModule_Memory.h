#pragma once

// This header pulls in the memory interface functions

#ifdef CPUMODULE_MEMORY_TEST

// For testing

#include "FMEM_test.h"

#else

#include "MemoryInterface.h"

// The legacy UAE filesystem trap integration is part of the WinFellow host
// layer and its headers are not present in the portable Linux build. Keep
// those dependencies on Windows while the filesystem trap is moved behind a
// frontend-neutral host interface.
#ifdef _WIN32
#include "uae2fell.h"
#include "autoconf.h"
#endif

#endif
