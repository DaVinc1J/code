// ******************************************************************
// ******************************************************************
// **                                                              **
// **             ORBITAL SIMULATOR !                              **
// **             this calculates between ALL bodies               **
// **                                                              **
//******************************************************************
// ******************************************************************

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <math.h>
#include <stdio.h>

// struct for OBJECTS !!!
//this WILL change!
typedef struct {
  // calculation value – to keep precision as rect.x is only ints
  double pos[2];
  // calculation value
  double vel[2];
  // visual value
  int size[2];
  // calculation value
  int mass;
  // visual value
  int colour[4];
} object;

void forceCalculation(object *first, object *second) {
  
  // funny pointer signal hehe
  double xDistance = second->pos[0] - first->pos[0];
  double yDistance = second->pos[1] - first->pos[1];

  // find the 
  double distanceSQRD = xDistance * xDistance + yDistance * yDistance;

  //to prevent dividing by zero
  if (distanceSQRD == 0) {
    return;
  }

  double angle = atan2(yDistance, xDistance);

  double force = (first->mass * second->mass) / (distanceSQRD);

  double xAcceleration, yAcceleration;

  xAcceleration = (force / first->mass) * cos(angle);
  yAcceleration = (force / first->mass) * sin(angle);
  first->vel[0] += xAcceleration;
  first->vel[1] += yAcceleration;

  xAcceleration = (force / second->mass) * cos(angle);
  yAcceleration = (force / second->mass) * sin(angle);
  second->vel[0] -= xAcceleration;
  second->vel[1] -= yAcceleration;

}

void doObjectInteractions(object *obj, 
                          int numObjs, 
                          SDL_Renderer *ren, 
                          const int *windowDimensionsCentre, 
                          int showSpecs, 
                          TTF_Font *font, 
                          SDL_Texture **textTextures, 
                          SDL_Rect *textRects) {

    double xRelative, yRelative;

    char buffer[256];

    for (int i = 0; i < numObjs; ++i) {
        for (int j = i + 1; j < numObjs; ++j) {
            forceCalculation(&obj[i], &obj[j]);
        }

        obj[i].pos[0] += obj[i].vel[0];
        obj[i].pos[1] += obj[i].vel[1];

        SDL_Rect rect;
        rect.w = obj[i].size[0];
        rect.h = obj[i].size[1];

        if (i == numObjs - 1) {
            rect.x = windowDimensionsCentre[0] - obj[i].size[0] / 2;
            rect.y = windowDimensionsCentre[1] - obj[i].size[1] / 2;
        } else {
            xRelative = obj[i].pos[0] - obj[numObjs - 1].pos[0];
            yRelative = obj[i].pos[1] - obj[numObjs - 1].pos[1];

            rect.x = (int)(windowDimensionsCentre[0] + xRelative - obj[i].size[0] / 2);
            rect.y = (int)(windowDimensionsCentre[1] + yRelative - obj[i].size[1] / 2);
        }

        if (showSpecs) {
            snprintf(buffer, sizeof(buffer), "C(%d, %d) V(%d, %d)", rect.x, rect.y, (int)obj[i].vel[0], (int)obj[i].vel[1]);

            if (textTextures[i] == NULL || textRects[i].x != rect.x || textRects[i].y != rect.y) {
                SDL_Surface *textSurface = TTF_RenderText_Solid(font, buffer, (SDL_Color){255, 255, 255, 255});

                if (textTextures[i] != NULL) {
                    SDL_DestroyTexture(textTextures[i]);
                }

                textTextures[i] = SDL_CreateTextureFromSurface(ren, textSurface);
                SDL_FreeSurface(textSurface);

                SDL_QueryTexture(textTextures[i], NULL, NULL, &textRects[i].w, &textRects[i].h);
            }

            textRects[i].x = rect.x + 10;
            textRects[i].y = rect.y - 10;

            SDL_RenderCopy(ren, textTextures[i], NULL, &textRects[i]);
        }

        SDL_SetRenderDrawColor(ren, obj[i].colour[0], obj[i].colour[1], obj[i].colour[2], obj[i].colour[3]);
        SDL_RenderFillRect(ren, &rect);
    }
}

int main() {

  // initialise and error handling, just in case
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }


  const int windowDimensions[] = {640, 480};
  int windowDimensionsCentre[] = {windowDimensions[0] / 2, windowDimensions[1] / 2};

  /* 
  create an SDL window !! 
  word at the end is for type of window
  0x00000004 = normal window
  0x00000020 = resizeable window
  */

  TTF_Font *font = TTF_OpenFont("VictorMonoNerdFont.ttf", 16);
  if (font == NULL) {
    printf("TT_OpenFont Error: %s\n", TTF_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow(
    "Test Window",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    windowDimensions[0],
    windowDimensions[1],
    0x00000004
  );

  // print an error in case the previous shit fucks the hell up !
  if (win == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // create an sdl renderer 
  // tbh i dont even know what this is but guy who wrote the tutorial i followed sure as hell does :D
  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  // you are NOT going to believe what this is ...
  // MORE ERROR HANDLING :)))))
  if (ren == NULL) {
    SDL_DestroyWindow(win);
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  char fpsBuffer[256];

  SDL_Color textColour = {255, 255, 255, 255};

  SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Orbital Gravity Simulation", textColour);
  if (textSurface == NULL) {
    printf("TTF_RenderText_Solid Error: %s\n", TTF_GetError());
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
  SDL_FreeSurface(textSurface);
  if (textTexture == NULL) {
    printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Rect textRect = {.x = 10, .y = 5};
  SDL_Rect fpsRect;

  SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);  // define an object 

  object objOne = {
    // rgba colour value
    .colour = {255, 255, 255, 255},
    // velocity, positive = right and up, negative = left and down
    .vel = {0, 10.0},
    // location on screen
    .pos = {100, 0},
    // for force calculations
    .mass = 1,
    // for visual display
    .size = {5, 5}
  };

  object objTwo = {
    .colour = {0, 255, 0, 255},
    .vel = {0.0, 9.0},
    .pos = {150, 0},
    .mass = 1,
    .size = {5, 5}
  };

  object objThree = {
    .colour = {255, 0, 255, 255},
    .vel = {0.0, 8.0},
    .pos = {200, 0},
    .mass = 1,
    .size = {5, 5}
  };

  object objFour = {
    .colour = {128, 128, 255, 255},
    .vel = {0.0, 7.0},
    .pos = {250, 0},
    .mass = 1,
    .size = {5, 5}
  };

    // Big object
  object objMain = {
    .colour = {255, 255, 0, 255},
    .vel = {0.0, 0.0},
    .pos = {0, 0},
    .mass = 10000,
    .size = {50, 50}
  };

  // list of all objects to be used for screen updating!
  // called obj cause i dont wanna write out 'objects' constantly :D
  object obj[] = {objOne, objTwo, objThree, objFour, objMain};

  // number of all objects to be used for screen updating!
  const int numObjs = sizeof(obj) / sizeof(obj[0]);

  // set the background colour: red, green, blue, alpha
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

  SDL_Texture *textTextures[numObjs];
  SDL_Rect textRects[numObjs];
  memset(textTextures, 0, sizeof(textTextures));

  for (int i = 0; i < numObjs; ++i) {
    textRects[i] = (SDL_Rect){0, 0, 0, 0};
  }

  Uint32 frameStart;
  int frameTime;
  int fps;

  // loop to keep the window open until it is closed by the user
  int quit = 0;
  int showSpecs = 0;

  // what is an SDL_Event? great question! stay curious guys
  SDL_Event event;

  while (!quit) {
    // start counting frame
    frameStart = SDL_GetTicks();

    // handle events!
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            quit = 1;
            break;
          case SDLK_SPACE:
            showSpecs = 1 - showSpecs;
            break;
          default:
            break;
        }
      }
    }
    
    // clear the previous frame
    SDL_RenderClear(ren);

    // calculate everything for the next frame 
    doObjectInteractions(obj, numObjs, ren, windowDimensionsCentre, showSpecs, font, textTextures, textRects);

    // render the title
    SDL_RenderCopy(ren, textTexture, NULL, &textRect);

    // finds tge frames that have passed from the loop
    frameTime = SDL_GetTicks() - frameStart;

    // frame calculation
    if (frameTime > 0) {
      fps = 1000.0 / frameTime;
    } else {
      fps = 0;
    }

    snprintf(fpsBuffer, sizeof(fpsBuffer), "FPS: %d", fps);

    SDL_Surface *fpsSurface = TTF_RenderText_Solid(font, fpsBuffer, textColour);

    SDL_Texture *fpsTexture = SDL_CreateTextureFromSurface(ren, fpsSurface);
    SDL_FreeSurface(fpsSurface);

    fpsRect.x = windowDimensions[0] - 100;
    fpsRect.y = 5;

    SDL_QueryTexture(fpsTexture, NULL, NULL, &fpsRect.w, &fpsRect.h);

    SDL_RenderCopy(ren, fpsTexture, NULL, &fpsRect);
    SDL_DestroyTexture(fpsTexture);

    SDL_RenderPresent(ren);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

  }

  for (int i = 0; i < numObjs; ++i) {
    if (textTextures[i] != NULL) {
      SDL_DestroyTexture(textTextures[i]);
    }
  }

  TTF_CloseFont(font);
  SDL_DestroyTexture(textTexture);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
