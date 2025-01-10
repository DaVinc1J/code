#ifndef UI_H
#define UI_H

#include "global.h"

typedef struct {
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Event event;
	TTF_Font *fonts[FONT_TYPES][FONT_SIZES];
} graphics;

typedef struct {
	SDL_Texture *text_texture;
	SDL_Rect text_rect;
} textures;

typedef struct {
	unsigned int quit: 1;
} keymap;

typedef struct {
	SDL_Colour main_obj;
	SDL_Colour border_obj;
	SDL_Colour content_text;
} colour_list;

typedef struct {
	SDL_Rect main_box;
	SDL_Rect border_box;
} layout_boxes;

typedef struct {
	unsigned int is_active : 1;
	unsigned int is_previewed : 1;
	char *input_text;
	char *preview_text;
	uint8_t preview_font;
	SDL_Colour preview_colour;
} input_specific;

typedef struct {
	char *label;
} text_specific;

typedef struct {
	colour_list *on_click;
	SDL_bool render_click;
	uint16_t click_timer;
	char *label;
} button_specific;

typedef enum {
	INPUT,
	TEXT,
	BUTTON
} rect_type;

typedef struct rect_common rect_common;

typedef struct {
	rect_common *common;
	void *data;
	rect_type type;
} generic_rect;

typedef struct rect_common {
	layout_boxes boxes;
	colour_list colours;
	textures textures;
	uint8_t font;
	uint8_t id;
	uint8_t priority;
	uint8_t children_count;
	generic_rect **children;
} rect_common;

typedef struct {
	generic_rect *active_input_rect;
	uint8_t cursor_pos;
	unsigned int render_cursor : 1;
	unsigned char is_visible[BITMAP_SIZE];
	unsigned char is_static[BITMAP_SIZE];
	unsigned char is_centred[BITMAP_SIZE];
	unsigned char needs_update[BITMAP_SIZE];
} global_values;

void set_state(unsigned char *bitmap, int id);
void clear_state(unsigned char *bitmap, int id);
int check_state(const unsigned char *bitmap, int id);

int font_text_width(TTF_Font *font, const char *text, int length);

void initialise_graphics(graphics *gfx, const char *font_paths[], const int font_sizes[]);

void exit_SDL(graphics *gfx, generic_rect **boxes, int *box_count);

#endif
