#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

  SDL_Init(SDL_INIT_TIMER);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *win =
      SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800,
                       600, SDL_WINDOW_OPENGL);

  SDL_GLContext *ren = SDL_GL_CreateContext(win);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  int running = 1;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(win);
  }

  SDL_GL_DeleteContext(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
