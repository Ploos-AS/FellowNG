#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace
{
  constexpr int DefaultWidth = 720;
  constexpr int DefaultHeight = 568;
}

int main(int argc, char **argv)
{
  const bool self_test = argc > 1 && std::strcmp(argv[1], "--self-test") == 0;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
  if (self_test)
  {
    flags |= SDL_WINDOW_HIDDEN;
  }

  SDL_Window *window = SDL_CreateWindow("FellowNG SDL3", DefaultWidth, DefaultHeight, flags);
  if (window == nullptr)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (self_test)
  {
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "FellowNG SDL3 frontend bootstrap: PASS\n";
    return EXIT_SUCCESS;
  }

  bool running = true;
  while (running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        running = false;
      }
    }

    SDL_Delay(1);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
