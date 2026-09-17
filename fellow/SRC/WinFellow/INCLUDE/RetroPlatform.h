#pragma once

// Legacy WinFellow sources still include RetroPlatform.h even though the
// platform service is now provided through WinFellow.Core/Service/IRetroPlatform.h
// and ICorePlatformFactory. Keep this compatibility header intentionally empty
// while the remaining legacy source set is migrated to the frontend-neutral
// platform interfaces.
