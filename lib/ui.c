#include <ui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <emu.h>
#include <bus.h>

static int scale = 4;


SDL_Window*   sdl_window;
SDL_Renderer* sdl_renderer;
SDL_Texture*  sdl_texture;
SDL_Surface*  sdl_surface;

SDL_Window*   sdl_dbg_window;
SDL_Renderer* sdl_dbg_renderer;
SDL_Texture*  sdl_dbg_texture;
SDL_Surface*  sdl_dbg_surface;


/*
 * According to PanDocs, Each tile (or character) has 8×8 pixels.
 * So, here are some code use magic number 8
 * example: 
 *              SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0,
 *                &sdl_dbg_window, &sdl_dbg_renderer);
 *          
*/

void ui_init() {
  SDL_Init(SDL_INIT_VIDEO);
  printf("SDL INIT\n");
  TTF_Init();
  printf("TTF INIT\n");

  SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdl_window, &sdl_renderer);

  SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0,
    &sdl_dbg_window, &sdl_dbg_renderer);

  sdl_dbg_surface = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale),
                                          (32 * 8 * scale) + (64 * scale), 32,
                                          0x00FF0000,
                                          0x0000FF00,
                                          0x000000FF,
                                          0xFF000000);

  sdl_dbg_texture = SDL_CreateTexture(sdl_dbg_renderer,
                                        SDL_PIXELFORMAT_ABGR8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        (16 * 8 * scale) + (16 * scale),
                                        (32 * 8 * scale) + (64 * scale));
  
  int x, y;
  SDL_GetWindowPosition(sdl_window, &x, &y);
  SDL_SetWindowPosition(sdl_dbg_window, x + SCREEN_WIDTH, y);
}

//                                        White    light gray   dark gray   black
static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

/*
 *
*/
void DisplayTile(SDL_Surface* surface, uint16_t addr, uint16_t tile_num, int x, int y) {
  SDL_Rect rect;

  for (int tile_y = 0; tile_y < 16; tile_y += 2) {
    uint8_t b1 = bus_read(addr + (tile_num * 16) + tile_y);
    uint8_t b2 = bus_read(addr + (tile_num * 16) + tile_y + 1);

    for (int bit = 7; bit >= 0; bit--) {
      uint8_t hi = !!(b1 & (1 << bit)) << 1;
      uint8_t lo = !!(b2 & (1 << bit));

      uint8_t color = hi | lo;

      rect.x = x + ((7 - bit) * scale);
      rect.y = y + (tile_y  / 2 * scale);
      rect.w = scale;
      rect.h = scale;

      SDL_FillRect(surface, &rect, tile_colors[color]);
    }
  }
}

void UpdateDebugWindow() {
  int x_draw = 0;
  int y_draw = 0;
  int tile_num = 0;

  SDL_Rect rc;
  rc.x = 0;
  rc.y = 0;
  rc.w = sdl_dbg_surface->w;
  rc.h = sdl_dbg_surface->h;
  SDL_FillRect(sdl_dbg_surface, &rc, 0xFF111111);


  uint16_t addr = 0x8000;
  // 384 tiles , 24 * 16

  for (int y = 0; y < 24; y++) {
    for (int x = 0; x < 16; x++) {
      DisplayTile(sdl_dbg_surface, addr, tile_num,
            x_draw + (x * scale), y_draw + (y * scale));
      x_draw += (8 * scale);
      tile_num++;
    }
    y_draw += (8 * scale);
    x_draw = 0;
  }

  SDL_UpdateTexture(sdl_dbg_texture, NULL, sdl_dbg_surface->pixels, sdl_dbg_surface->pitch);
  SDL_RenderClear(sdl_dbg_renderer);
  SDL_RenderCopy(sdl_dbg_renderer, sdl_dbg_texture, NULL, NULL);
  SDL_RenderPresent(sdl_dbg_renderer);
}

void UiUpdate() {
  UpdateDebugWindow();
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