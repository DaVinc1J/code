#include "ui.h"
#include "global.h"

// bitmap stuff
// set state
void set_state(unsigned char *bitmap, int id) {
  bitmap[id / 8] |= (1 << (id % 8));
}

// clear state
void clear_state(unsigned char *bitmap, int id) {
  bitmap[id / 8] &= ~(1 << (id % 8));
}

// check state
int check_state(const unsigned char *bitmap, int id) {
  return (bitmap[id / 8] & (1 << (id % 8))) != 0;
}

// initialise objects
void initialise_graphics(graphics *gfx, const char *font_paths[],
                         const int font_sizes[]) {
  SDL_Init(SDL_INIT_TIMER);
  TTF_Init();
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
  SDL_StartTextInput();

  for (font_enum font = 0; font < FONT_TYPES; font++) {
    for (font_size_enum size = 0; size < FONT_SIZES; size++) {
      gfx->fonts[font][size] = TTF_OpenFont(font_paths[font], font_sizes[size]);
    }
  }

  gfx->win =
      SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WIDTH, HEIGHT, 0x00000020);

  gfx->ren = SDL_CreateRenderer(
      gfx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

int font_text_width(TTF_Font *font, const char *text, int length) {
  if (length <= 0) {
    return 0;
  }

  char *temp = (char *)malloc(length + 1);
  if (!temp) {
    return 0;
  }

  strncpy(temp, text, length);
  temp[length] = '\0';

  int width = 0;
  if (TTF_SizeText(font, temp, &width, NULL) != 0) {
    free(temp);
    return 0;
  }
  free(temp);
  return width;
}

// free objects
void exit_SDL(graphics *gfx, generic_rect **rects, int *rect_count) {
  SDL_StopTextInput();

  SDL_DestroyRenderer(gfx->ren);
  gfx->ren = NULL;

  SDL_DestroyWindow(gfx->win);
  gfx->win = NULL;

  for (font_enum font = 0; font < FONT_TYPES; font++) {
    for (font_size_enum size = 0; size < FONT_SIZES; size++) {
      TTF_CloseFont(gfx->fonts[font][size]);
    }
  }

  for (int i = 0; i < *rect_count; i++) {
    if (rects[i]) {
      free(rects[i]->data);
      free(rects[i]->common);
      free(rects[i]);
    }
  }
  free(rects);

  TTF_Quit();
  SDL_Quit();
}
