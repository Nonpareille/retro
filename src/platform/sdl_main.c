// "Portable" platform entry point using SDL and the standard C library.

#include <SDL2/SDL.h>
#include "../../test.h"

// TODO: put these elsewhere
#define bool int
#define false 0
#define true 1
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) < (y)) ? (y) : (x))
typedef uint8_t byte;
#define kb(x) ((size_t)(x) * 1024)
#define mb(x) (kb(x) * 1024)
#define gb(x) (mb(x) * 1024)

// TODO: no floating point
const float viewport_ratio = 320.f/224.f;

typedef enum ControllerState {
  Button_A        = 1 << 0,
  Button_X        = 1 << 1,
  Button_L        = 1 << 2,
  Button_R        = 1 << 3,
  Button_B        = 1 << 4,
  Button_Y        = 1 << 5,
  Button_Start    = 1 << 6,
  Button_Select   = 1 << 7,
  Direction_Up    = 1 << 8,
  Direction_Down  = 1 << 9,
  Direction_Left  = 1 << 10,
  Direction_Right = 1 << 11,
  State_Active    = 1 << 15,

  CLEAR_STATE     = 0x1000
} ControllerState;

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
                              700, 550, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
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

  struct {
    byte video_ram[mb(1)];
    uint16_t controllers[4]; // ControllerState
    //TODO: tile map layers
    //TODO: palette
    //TODO: object attribute memory
    //TODO: scroll registers
    //TODO: mode 7 registers
    //TODO: fixed point multiplication/division results
    //TODO: "interrupt" function pointers
  } registers = {};


  memcpy(registers.video_ram, data_test_bmp, data_test_bmp_len);

  SDL_Event event;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 2;
  bool quit = false;
  uint64_t freq = SDL_GetPerformanceFrequency();
  uint64_t now, last = SDL_GetPerformanceCounter();
  while (quit == false) {
    // clear the frame state
    for (int i = 0; i < 4; ++i) {
      registers.controllers[i] &= CLEAR_STATE;
    }

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
          static int keyboard_player = -1;
          // ControllerState controller = handleControllerEvent(KeyControllerMap(event.key));
          // if (keyboard_active == -1) {
          //   for (int i = 0; i < 4; ++i) {
          //     if (registers.controllers[i] & ControllerState::State_Active == 0) {
          //       keyboard_player = i;
          //       break;
          //     }
          //   }
          // }
          // registers.controllers[keyboard_player] |= controller;
        } break;
      }
    }

    // renderer will go here
    int pixel_off_x = device.surface->clip_rect.x + (device.surface->clip_rect.w % 320) / 2;
    int pixel_off_y = device.surface->clip_rect.y + (device.surface->clip_rect.h % 224) / 2;
    int pixel_width = device.surface->clip_rect.w / 320;
    int pixel_height = device.surface->clip_rect.h / 224;
    for (int i = 0; i < 320*224; ++i)
    {
      SDL_Rect pixel = {
        pixel_off_x + (pixel_width * (i % 320)),
        pixel_off_y + (pixel_height * (224 - (i / 320))),
        pixel_width, pixel_height
      };
      r = registers.video_ram[(i*3)+2];
      g = registers.video_ram[(i*3)+1];
      b = registers.video_ram[(i*3)+0];
      SDL_FillRect(device.surface, &pixel, SDL_MapRGB(device.surface->format, r, g, b));
    }
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
