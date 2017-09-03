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

typedef struct Device {
  SDL_Window *window;
  SDL_Surface *surface;
} Device;
bool deviceIsEmpty(Device d) { return !d.window || !d.surface; }

SDL_Rect computeClipRect(int w, int h, float ratio) {
  SDL_Rect result = {
    max(0, (w - (h * ratio)) / 2),
    max(0, (h - (w * 1.f/ratio)) / 2),
    min(w, h * ratio),
    min(h, w * 1.f/ratio),
  };
  return result;
}

Device handleWindowEvent(SDL_WindowEvent event) {
  Device result = {};
  result.window = SDL_GetWindowFromID(event.windowID);
  if (!result.window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not retrieve window #%d", event.windowID);
    return result;
  }
  result.surface = SDL_GetWindowSurface(result.window);
  if (!result.surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not retrieve surface from window #%d", event.windowID);
    return result;
  }
  switch (event.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
      SDL_SetClipRect(result.surface, NULL);
      SDL_FillRect(result.surface, NULL, SDL_MapRGB(result.surface->format, 0x00, 0x00, 0x00));

      SDL_Rect clip_rect = computeClipRect(event.data1, event.data2, viewport_ratio);
      SDL_SetClipRect(result.surface, &clip_rect);
    } break;
  }
  return result;
}

int main(int arg_c, char *arg_v[])
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
    return 1;
  }

  // set up device (window and surface)
  Device device; {
    device.window = SDL_CreateWindow("retro engine demo",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              512, 224, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if (!device.window)
    {
      // handle error
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s\n", SDL_GetError());
      return 1;
    }

    device.surface = SDL_GetWindowSurface(device.window);
    SDL_FillRect(device.surface, NULL, SDL_MapRGB(device.surface->format, 0x00, 0x00, 0x00));

    SDL_Rect clip_rect = computeClipRect(device.surface->w, device.surface->h, viewport_ratio);
    SDL_SetClipRect(device.surface, &clip_rect);
  }

  SDL_Event event;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 2;
  bool quit = false;
  uint64_t freq = SDL_GetPerformanceFrequency();
  uint64_t now, last = SDL_GetPerformanceCounter();
  while (quit == false) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: {
          quit = true;
        } break;
        case SDL_WINDOWEVENT: {
          device = handleWindowEvent(event.window);
          if (deviceIsEmpty(device)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to handle window event!");
            return 1;
          }
        } break;
        case SDL_KEYDOWN: {

        } break;
      }
    }

    // renderer will go here
    SDL_FillRect(device.surface, NULL, SDL_MapRGB(device.surface->format, r++, g+=2, b+=3));
    SDL_UpdateWindowSurface(device.window);

    // for now we estimate a "frame rate" by just waiting some milliseconds.
    now = SDL_GetPerformanceCounter();
    double wait = ((now - last) / (double)(freq)) * 1000.0;
    if (wait < 60.0) {
      //SDL_Log("waiting %f ms", 60.0 - wait);
      SDL_Delay(60.0 - wait);
      double test = (60.0 - wait) - (((double)SDL_GetPerformanceCounter() - now) / (double)freq) * 1000.0;
      if (test < -2.0) {
        SDL_Log("delayed way too long!, %f ms", test);
      }
    }
    last = now;
  }

  SDL_DestroyWindow(device.window);
  SDL_Quit();
  return 0;
}
