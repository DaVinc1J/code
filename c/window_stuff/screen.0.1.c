#include "global.h"
#include "graphics.h"
#include "ui.h"

int font_text_width(TTF_Font *font, const char *text, int length) {
	if (length <= 0) {
		return 0;
	}

	char temp[MAX_INPUT_CHARS];
	strncpy(temp, text, length);
	int width = 0;
	TTF_SizeText(font, temp, &width, NULL);
	return width;
}

void initialise(graphics *gfx) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	TTF_Init();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	SDL_StartTextInput();

	gfx->fonts[1] = TTF_OpenFont("/Users/tristanlowe/nvim/fonts/AnonymiceProNerdFont.ttf", 55);

	gfx->win = SDL_CreateWindow(
		"",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT,
		0x00000020
	);

	gfx->ren = SDL_CreateRenderer(
		gfx->win,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
}

void check_events(graphics *gfx, keymap *keys, assets *boxes, asset_count *box_count) {
	while (SDL_PollEvent(&gfx->event)) {
		
		if (gfx->event.type == SDL_QUIT) {
			keys->quit = 1;
		} 

		if (gfx->event.type == SDL_KEYDOWN) {
			switch (gfx->event.key.keysym.sym) {
				case SDLK_ESCAPE:
					keys->quit = 1;
					break;
				default:
					break;
			}

			for (int i = 0; i < box_count->input_box_count; i++) {
				text_input *text = &(boxes->input_boxes[i].input);

				if (!text) {
					continue;
				}

				if (!text->active) {
					continue;
				}

				switch (gfx->event.key.keysym.sym) {
					case SDLK_BACKSPACE:
						if (text->cursor_pos > 0) {

							memmove(&text->input_text[text->cursor_pos - 1], 
							 &text->input_text[text->cursor_pos], 
							 strlen(text->input_text) - text->cursor_pos + 1);

							text->cursor_pos--;
						}
						break;

					case SDLK_LEFT:
						if (text->cursor_pos > 0) {

							text->cursor_pos--;
						}
						break;

					case SDLK_RIGHT:
						if (text->cursor_pos < strlen(text->input_text)) {

							text->cursor_pos++;
						}
						break;

					default:
						break;
				}	
			}
		}

		if (gfx->event.type == SDL_TEXTINPUT) {
			for (int i = 0; i < box_count->input_box_count; i++) {
				text_input *text = &(boxes->input_boxes[i].input);

				if (!text) {
					continue;
				}

				if (!text->active) {
					continue;
				}

				if (strlen(text->input_text) + strlen(gfx->event.text.text) > MAX_INPUT_CHARS) {
					continue;
				}

				memmove(&text->input_text[text->cursor_pos + strlen(gfx->event.text.text)],
						&text->input_text[text->cursor_pos],
						strlen(text->input_text) - text->cursor_pos + 1);

				memcpy(&text->input_text[text->cursor_pos],
					 gfx->event.text.text,
					 strlen(gfx->event.text.text));

				text->cursor_pos += strlen(gfx->event.text.text);

			}
		}


		if (gfx->event.type == SDL_MOUSEBUTTONDOWN) {

			int x, y;

			SDL_GetMouseState(&x, &y);

			for (int i = 0; i < box_count->input_box_count; i++) {
				text_input *text = &(boxes->input_boxes[i].input);

				if (!text) {
					continue;
				}

				if (SDL_PointInRect(&(SDL_Point){x, y}, &(boxes->input_boxes[i].boxes.border_box))) {
					text->active = 1;
				} else {
					text->active = 0;
				}
			}

			for (int i = 0; i < box_count->button_box_count; i++) {

				button_rect button = boxes->button_boxes[i];

				if (SDL_PointInRect(&(SDL_Point){x, y}, &(button.boxes.border_box))) {
					printf("Button #%d clicked", button.id);
					printf("Value of %s", button.input->input_text);
					
					memset(button.input->input_text, 0, (sizeof(button.input->input_text)));

					button.input->input_text[0] = ' ';

					button.input->cursor_pos = 0;	

				}
			}
		}
	}
}

void render_objects(graphics *gfx, assets *boxes, asset_count *box_count) {
  
		SDL_SetRenderDrawColor(gfx->ren, 255, 255, 255, 255);
    SDL_RenderClear(gfx->ren);

	for (int i = 0; i < box_count->input_box_count; i++) {
        input_rect *box = &boxes->input_boxes[i];

        SDL_SetRenderDrawColor(gfx->ren, 
                               box->colours.border_colour.r, 
                               box->colours.border_colour.g, 
                               box->colours.border_colour.b, 
                               box->colours.border_colour.a);
        SDL_RenderFillRect(gfx->ren, &box->boxes.border_box);

        SDL_SetRenderDrawColor(gfx->ren, 
                               box->colours.main_colour.r, 
                               box->colours.main_colour.g, 
                               box->colours.main_colour.b, 
                               box->colours.main_colour.a);
        SDL_RenderFillRect(gfx->ren, &box->boxes.main_box);
    }

    SDL_RenderPresent(gfx->ren);
}

void exit_SDL(graphics *gfx) {
	SDL_StopTextInput();

	SDL_DestroyRenderer(gfx->ren);
	gfx->ren = NULL;

	SDL_DestroyWindow(gfx->win);
	gfx->win = NULL;

	TTF_Quit();
	SDL_Quit();
}

int main() {
	graphics gfx;

	keymap keys = {0};

	initialise(&gfx);

	asset_count box_count = {0};
	box_count.input_box_count = 1;
	box_count.text_box_count = 0;
	box_count.button_box_count = 0;

	assets boxes;
	boxes.input_boxes = (input_rect *)malloc(sizeof(input_rect) * box_count.input_box_count);
	boxes.text_boxes = (text_rect *)malloc(sizeof(text_rect) * box_count.text_box_count);
	boxes.button_boxes = (button_rect *)malloc(sizeof(button_rect) * box_count.button_box_count);

	boxes.input_boxes[0] = (input_rect){
		.boxes = {
			.main_box = {100, 100, 200, 200},
			.border_box = {95, 95, 210, 210}
		},
		.colours = {
			.main_colour = {255, 255, 255, 255},
			.border_colour = {0, 0, 0, 255},
			.text_colour = {0, 0, 0, 255}
		},
		.texture = {
			.text_rect = {100, 100, 400, 200},
		},
		.input = {
			.cursor_pos = 0,
			.cursor_timer = 0,
			.render_cursor = 0,
			.active = 0,
			.input_text[0] = ' '
		},
		.font = gfx.fonts[1],
		.scale = 1.0f,
		.visible = 1,
		.is_static = 1,
		.id = 1
	};

	while (!keys.quit) {
		check_events(&gfx, &keys, &boxes, &box_count);
    render_objects(&gfx, &boxes, &box_count);
	}

	exit_SDL(&gfx);
	return 0;

}
