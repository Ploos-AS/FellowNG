#pragma once

// Legacy WinFellow sources include the UAE filesystem header using the
// lowercase Windows spelling. Linux filesystems are case-sensitive, while the
// imported UAE header is stored as FILESYS.H. Keep the legacy source unchanged
// and provide the portable spelling bridge here.
#include "../../uae/FILESYS.H"
