#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct object {
  int colour[4];
  double pos[2];
  double vel[2];
  int mass;
  int id;
  int parentId;
  struct object **children;
  int numChildren;
} object;

typedef struct {
  int quit;
  int showSpecs;
  int menu;
} options;

typedef struct {
  TTF_Font *font;
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Event event;
} graphics;

object *createObject(int colour[4], double pos[2], double vel[2], int mass,
                     int id, int parentId) {
  object *obj = (object *)malloc(sizeof(object));

  obj->mass = mass;
  obj->id = id;
  obj->parentId = parentId;
  obj->numChildren = 0;
  obj->children = NULL;

  for (int i = 0; i < 4; i++) {
    obj->colour[i] = colour[i];
  }

  for (int i = 0; i < 2; i++) {
    obj->pos[i] = pos[i];
    obj->vel[i] = vel[i];
  }

  return obj;
}

void freeObject(object *obj) {
  if (obj->children != NULL) {
    freeObject(obj->children);
  }
  free(obj);
}

void addChild(object *parent, object *child) {
  parent->numChildren++;
  parent->children = (object **)realloc(parent->children,
                                        parent->numChildren * sizeof(object *));
  parent->children[parent->numChildren - 1] = child;
}

void initialise(graphics *gfx, int width, int height) {

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  TTF_Init();

  gfx->font = TTF_OpenFont("VictorMonoNerdFont.ttf", 14);

  gfx->win =
      SDL_CreateWindow("...", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, 0x00000004);

  gfx->ren = SDL_CreateRenderer(
      gfx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void checkEvents(graphics *gfx, options *option) {

  while (SDL_PollEvent(&gfx->event)) {

    // exiting the window
    if (gfx->event.type == SDL_QUIT) {
      option->quit = 1;
    }

    // key presses
    if (gfx->event.type == SDL_KEYDOWN) {
      switch (gfx->event.key.keysym.sym) {
      case SDLK_ESCAPE:
        option->menu = 1;
        break;
      case SDLK_SPACE:
        option->showSpecs = 1;
        break;
      default:
        break;
      }
    }

    // handles mouse clicking
    if (gfx->event.type == SDL_MOUSEBUTTONDOWN) {
      if (gfx->event.button.button == SDL_BUTTON_LEFT) {
        // add stuff here
      }
    }
  }
}

void exitSDL(graphics *gfx) {
  TTF_CloseFont(gfx->font);
  SDL_DestroyRenderer(gfx->ren);
  SDL_DestroyWindow(gfx->win);
  TTF_Quit();
  SDL_Quit();
}

void renderObjects(graphics *gfx, options *option) {
  SDL_RenderClear(gfx->ren);

  // do object rendering here

  SDL_RenderPresent(gfx->ren);
}

int main() {

  // SDL Initialising

  const int width = 800;
  const int height = 600;

  graphics gfx;

  initialise(&gfx, width, height);

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

  //

  // Object Creation

  object moon = {.colour = {189, 195, 199, 255},
                 .pos = {600, 300},
                 .vel = {0, 0},
                 .mass = 7,
                 .id = 1,
                 .gravInfluence = 2};

  object earth = {.colour = {0, 0, 163, 255},
                  .pos = {200, 300},
                  .vel = {0, 0},
                  .mass = 600,
                  .id = 2,
                  .gravInfluence = 0};

  object obj[] = {moon, earth};

  const int numObjs = sizeof(obj) / sizeof(obj[1]);

  //

  // options stuff

  options option = {.quit = 0, .showSpecs = 0, .menu = 0};

  //

  while (!option.quit) {

    checkEvents(&gfx, &option);

    renderObjects(&gfx, &option);
  }

  exitSDL(&gfx);

  return 0;
}
