#include <chrono>
#include <cstdlib>
#include <filesystem>

#include "CustomChipset/RegisterUtility.h"
#include "CustomChipset/Registers.h"
#include "Platform/StdClock.h"
#include "Platform/StdFileSystem.h"

namespace
{
  bool check(bool condition)
  {
    return condition;
  }
}

int main()
{
  CustomChipset::Registers registers;
  CustomChipset::RegisterUtility utility(registers);

  if (!check(utility.IsLoresEnabled())) return EXIT_FAILURE;
  if (!check(!utility.IsHiresEnabled())) return EXIT_FAILURE;
  if (!check(utility.GetEnabledBitplaneCount() == 0)) return EXIT_FAILURE;
  if (!check(!utility.IsMasterDMAEnabled())) return EXIT_FAILURE;

  registers.BplCon0 = static_cast<uint16_t>(0x8000 | 0x4000 | 0x0800 | 0x0400 | 0x0004);
  registers.BplCon2 = 0x0040;
  registers.DmaConR = static_cast<uint16_t>(0x0200 | 0x0100 | 0x0010 | 0x0400);

  if (!check(utility.IsHiresEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsDualPlayfieldEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsHAMEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsInterlaceEnabled())) return EXIT_FAILURE;
  if (!check(utility.GetEnabledBitplaneCount() == 4)) return EXIT_FAILURE;
  if (!check(utility.IsPlayfield2PriorityEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsMasterDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsMasterDMAAndBitplaneDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsDiskDMAEnabled())) return EXIT_FAILURE;
  if (!check(utility.IsBlitterPriorityEnabled())) return EXIT_FAILURE;

  FellowNG::Platform::StdClock clock;
  const auto before = clock.Now();
  clock.SleepFor(std::chrono::milliseconds(1));
  const auto after = clock.Now();
  if (!check(after >= before)) return EXIT_FAILURE;

  FellowNG::Platform::StdFileSystem file_system;
  const auto current = file_system.Stat(std::filesystem::current_path());
  if (!check(current.exists && current.directory)) return EXIT_FAILURE;
  if (!check(file_system.Absolute(".").is_absolute())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
