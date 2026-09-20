#pragma once

// Renderer.cpp still queries the legacy RetroPlatform singleton directly in
// its output-scale helpers even when RETRO_PLATFORM is not enabled. Portable
// SDL hosts are normal interactive hosts, so provide only the narrow legacy
// queries required while the renderer is being detached from RetroPlatform.
#if !defined(RETRO_PLATFORM)
struct FellowNGPortableRendererHostMode
{
  constexpr bool GetHeadlessMode() const noexcept
  {
    return false;
  }

  // Kept for compile-time compatibility with the legacy headless branch in
  // drawGetOutputScaleFactor(). Interactive SDL hosts never take that branch.
  constexpr unsigned int GetDisplayScale() const noexcept
  {
    return 1U;
  }
};

inline constexpr FellowNGPortableRendererHostMode RP{};
#endif
