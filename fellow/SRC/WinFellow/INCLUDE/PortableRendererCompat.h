#pragma once

// Renderer.cpp still queries the legacy RetroPlatform singleton directly in
// its output-clip helpers even when RETRO_PLATFORM is not enabled.  Portable
// SDL hosts are normal interactive hosts, so provide the narrow legacy query
// required while the renderer is being detached from RetroPlatform.
#if !defined(_WIN32) && !defined(RETRO_PLATFORM)
struct FellowNGPortableRendererHostMode
{
  constexpr bool GetHeadlessMode() const noexcept
  {
    return false;
  }
};

inline constexpr FellowNGPortableRendererHostMode RP{};
#endif
