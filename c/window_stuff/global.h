
#ifndef GLOBAL_H
#define GLOBAL_H

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 600
#define WIDTH 600
#define MAX_INPUT_CHARS (64 + 2)
#define MAX_CHILDREN 4
#define MAX_LABEL_CHARS (16 + 2)

#define MAX_IDS 255
#define BITMAP_SIZE ((MAX_IDS + 7) / 8)

#define SDL_SetRenderDrawColour SDL_SetRenderDrawColor
typedef SDL_Color SDL_Colour;

typedef enum {
	FONT_SIZE_EXTRA_SMALL,
	FONT_SIZE_SMALL,
	FONT_SIZE_MEDIUM,
	FONT_SIZE_LARGE,
	FONT_SIZES
} font_size_enum;

typedef enum {
	FONT_REGULAR,
	FONT_BOLD,
	FONT_ITALIC,
	FONT_BOLD_ITALIC,
	FONT_TYPES
} font_enum;

#endif
