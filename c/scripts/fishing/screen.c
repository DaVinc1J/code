#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>

/*

Important Parts to edit when using this file

 • link to font file, will crash otherwise

*/

#define HEIGHT 400
#define WIDTH 400
#define MAX_INPUT_CHARS 8 + 2

typedef struct {
  int quit;
} keymap;

typedef struct {
  TTF_Font *font;
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Event event;
} graphics;

typedef struct {
  char inputText[MAX_INPUT_CHARS];
  int cursorPos;
  SDL_bool renderCursor;
  Uint32 cursorTimer;
} textInput;

typedef struct {
  int width;
  int height;
  SDL_Color textColor;
  TTF_Font *font;
  int scale;
  textInput *textInput;
  SDL_Rect shape;
} box;

int fontTextWidth(TTF_Font *font, const char *text, int length) {
  if (length <= 0) {
    return 0;
  }

  char temp[MAX_INPUT_CHARS];
  strncpy(temp, text, length);
  temp[length] = '\0';
  int width = 0;
  TTF_SizeText(font, temp, &width, NULL);
  return width;
}

void initialise(graphics *gfx) {
  SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

  TTF_Init();

  SDL_StartTextInput();

  gfx->font = TTF_OpenFont(
      "/Users/tristanlowe/nvim/fonts/JetBrainsMonoNerdFont.ttf", /* link to font
                                                                    file */
      50);                                                       /* Font size */

  gfx->win = SDL_CreateWindow(
      "Script Login", /* Name of window */
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, /* Where the window is position on screen */
      WIDTH, HEIGHT,          /* pretty obvious */
      0x00000004);            /* makes the window resizeable */

  gfx->ren = SDL_CreateRenderer(
      gfx->win, /* Window where the renderer is displayed */
      -1,       /* initilaise everything important */
      SDL_RENDERER_ACCELERATED |
          SDL_RENDERER_PRESENTVSYNC); /* Hardware acceleration + synchronised
                                         refresh */
}

void checkEvents(graphics *gfx, keymap *keys, textInput *textInput) {

  while (SDL_PollEvent(
      &gfx->event)) { /* If an event is happening, i.e., key presses */

    if (gfx->event.type == SDL_QUIT) {
      keys->quit = 1; /* If you click exit it will exit the app */
    } else if (gfx->event.type == SDL_KEYDOWN) {
      switch (gfx->event.key.keysym.sym) {
      case SDLK_ESCAPE:
        keys->quit = 1; /* If you press escape it will exit the app */
        break;
      case SDLK_BACKSPACE:
        if (textInput->cursorPos > 0) {
          memmove(&textInput->inputText[textInput->cursorPos - 1],
                  &textInput->inputText[textInput->cursorPos],
                  strlen(textInput->inputText) - textInput->cursorPos + 1);
          textInput->cursorPos--;
        }
        break;
      case SDLK_RETURN:
        printf("User Input:%s\n", textInput->inputText);
        memset(textInput->inputText, 0, sizeof(textInput->inputText));
        textInput->inputText[0] = ' ';
        textInput->cursorPos = 0;
        break;
      case SDLK_LEFT:
        if (textInput->cursorPos > 0) {
          textInput->cursorPos--;
        }
        break;
      case SDLK_RIGHT:
        if (textInput->cursorPos < strlen(textInput->inputText)) {
          textInput->cursorPos++;
        }
        break;
      default:
        break;
      }
    } else if (gfx->event.type == SDL_TEXTINPUT) {
      if (strlen(textInput->inputText) + strlen(gfx->event.text.text) <
          MAX_INPUT_CHARS) {
        memmove(&textInput->inputText[textInput->cursorPos +
                                      strlen(gfx->event.text.text)],
                &textInput->inputText[textInput->cursorPos],
                strlen(textInput->inputText) - textInput->cursorPos + 1);
        memcpy(&textInput->inputText[textInput->cursorPos],
               gfx->event.text.text, strlen(gfx->event.text.text));
        textInput->cursorPos += strlen(gfx->event.text.text);
      }
    }
  }
}

void renderObjects(graphics *gfx, keymap *keys, textInput *textInput) {

  SDL_SetRenderDrawColor(
      gfx->ren,      /* Set drawing colour based on below values */
      255, 255, 255, /* RGB values */
      255);          /* Alpha value */

  SDL_RenderClear(gfx->ren);

  /* Manually define objects rn */
  /* In the future set up a system where i can do it in another file */
  SDL_Rect textBoxRect = {(WIDTH - 256) / 2, /* Centres it */
                          (HEIGHT - 56) / 2, /* Centres it */
                          256,               /* Width */
                          56};               /* Height */

  SDL_SetRenderDrawColor(gfx->ren, 180, 180, 180,
                         100); /* Set draw colour to black */
  SDL_RenderFillRect(gfx->ren,
                     &textBoxRect); /* Draw border around text input */

  SDL_Rect textBackgroundRect = {(WIDTH - 250) / 2, (HEIGHT - 50) / 2, 250, 50};

  SDL_SetRenderDrawColor(gfx->ren, 200, 200, 200, 100);
  SDL_RenderFillRect(gfx->ren, &textBackgroundRect);

  SDL_Color textColor = {0, 0, 0, 255};

  SDL_Surface *textSurface =
      TTF_RenderUTF8_Blended(gfx->font, textInput->inputText, textColor);

  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(gfx->ren, textSurface);

  SDL_Rect srcRect = {0, 0, textSurface->w, textSurface->h};

  int scale = 1;

  SDL_Rect textRect = {textBoxRect.x +
                           15 + // 15 is half the width of the font in pixels
                           (textBoxRect.w - textSurface->w) / 2,
                       textBoxRect.y + (textBoxRect.h - textSurface->h) / 2,
                       textSurface->w / scale, textSurface->h / scale};

  SDL_RenderCopyEx(gfx->ren, textTexture, &srcRect, &textRect, 0, NULL,
                   SDL_FLIP_NONE);

  Uint32 currentTime = SDL_GetTicks();
  if (currentTime - textInput->cursorTimer >= 500) {
    textInput->renderCursor = !textInput->renderCursor;
    textInput->cursorTimer = currentTime;
  }

  if (textInput->renderCursor) {
    int cursorX = textRect.x + fontTextWidth(gfx->font, textInput->inputText,
                                             textInput->cursorPos) /
                                   scale;
    SDL_SetRenderDrawColor(gfx->ren, 0, 0, 0, 255);
    SDL_RenderDrawLine(gfx->ren, cursorX, textBackgroundRect.y, cursorX,
                       textBackgroundRect.y + textBackgroundRect.h);
  }

  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);

  SDL_RenderPresent(gfx->ren);
}

void exitSDL(graphics *gfx) {
  SDL_StopTextInput();

  TTF_CloseFont(gfx->font);
  gfx->font = NULL;

  SDL_DestroyRenderer(gfx->ren);
  gfx->ren = NULL;

  SDL_DestroyWindow(gfx->win);
  gfx->win = NULL;

  TTF_Quit();
  SDL_Quit();
}

int main() {

  graphics gfx; /* Initialise graphics struct */

  textInput textInput = {.inputText = " ",
                         .cursorPos = 0,
                         .renderCursor = SDL_TRUE,
                         .cursorTimer = SDL_GetTicks()};

  keymap keys = {0};

  initialise(&gfx); /* Initialise graphics */

  while (!keys.quit) {
    checkEvents(&gfx, &keys, &textInput);
    renderObjects(&gfx, &keys, &textInput);
  }

  exitSDL(&gfx);

  return 0;
}
