#pragma once

#include "Driver/ISoundDriver.h"

namespace FellowNG::Driver
{
  class NullSoundDriver final : public ::ISoundDriver
  {
  public:
    void Play(int16_t *, int16_t *, uint32_t) override {}
    void PollBufferPosition() override {}
    bool SetCurrentSoundDeviceVolume(int) override { return true; }
    bool EmulationStart(SoundDriverRuntimeConfiguration) override
    {
      _initialized = true;
      return true;
    }
    void EmulationStop() override { _initialized = false; }
    bool IsInitialized() override { return _initialized; }

  private:
    bool _initialized = false;
  };
}
