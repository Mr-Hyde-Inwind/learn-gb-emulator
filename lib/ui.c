#include <ui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <emu.h>


SDL_Window* sdl_window;
SDL_Renderer* sdl_renderer;
SDL_Texture* sdl_texture;
SDL_Surface* sdl_surface;

void ui_init() {
  SDL_Init(SDL_INIT_VIDEO);
  printf("SDL INIT\n");
  TTF_Init();
  printf("TTF INIT\n");

  SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdl_window, &sdl_renderer);
}

void ui_handle_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e) > 0) {
    // SDL_UpdateWindowSurface(sdl_window);
    // SDL_UpdateWindowSurface(sdl_trace_window);
    // SDL_UpdateWindowSurface(sdl_debug_window);


    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
      emu_get_context()->die = true;
    }
  }
}