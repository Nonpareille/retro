// "Portable" platform entry point using SDL and the standard C library.

#include <SDL2/SDL.h>

// TODO: put these elsewhere
#define bool int
#define false 0
#define true 1
#define min(x, y) ((x) < (y)) ? (x) : (y)
#define max(x, y) ((x) < (y)) ? (y) : (x)

// TODO: no floating point
const float viewport_ratio = 320.f/224.f;

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
                              512, 224, SDL_WINDOW_SHOWN);
    if (!device.window)
    {
      // handle error
      printf("Couldn't create window: %s\n", SDL_GetError());
      return 1;
    }

    device.surface = SDL_GetWindowSurface(device.window);
    SDL_FillRect(device.surface, NULL, SDL_MapRGB(device.surface->format, 0x00, 0x00, 0x00));

    int clip_width = device.surface->h * viewport_ratio;
    int clip_height = device.surface->w * 1.f / viewport_ratio;
    SDL_Rect clip_rect = {
      max(0, (device.surface->w - clip_width) / 2), // x
      max(0, (device.surface->h - clip_height) / 2), // y
      min(device.surface->w, clip_width), // w
      min(device.surface->h, clip_height) // h
    };
    SDL_SetClipRect(device.surface, &clip_rect);
  }

  SDL_Event event;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        quit = true;
      } break;
      case SDL_KEYDOWN: {
      }
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
