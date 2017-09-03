// "Portable" platform entry point using SDL and the standard C library.

#include <SDL2/SDL.h>

#define bool int
#define false 0
#define true 1

int main(int arg_c, char *arg_v[])
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

// set up device (window and surface)
  struct {
    SDL_Window *window;
    SDL_Surface *surface;
  } device; {
    device.window = SDL_CreateWindow("retro engine demo",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              1024, 1024, SDL_WINDOW_SHOWN);
    if (!device.window)
    {
      // handle error
      printf("Couldn't create window: %s\n", SDL_GetError());
      return 1;
    }

    device.surface = SDL_GetWindowSurface(device.window);
  }

  SDL_Event event;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        quit = true;
      } break;
      }
    }

    // renderer will go here
    SDL_FillRect(device.surface, NULL, SDL_MapRGB(device.surface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(device.window);
  }

  SDL_DestroyWindow(device.window);
  SDL_Quit();
  return 0;
}
