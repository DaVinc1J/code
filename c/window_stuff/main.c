#include "global.h"
#include "ui.h"
#include "fonts.h"

SDL_Texture* update_texture(graphics *gfx, TTF_Font *font, char *text, SDL_Colour *text_colour, SDL_Colour *background_colour) {
	SDL_Surface *surface = TTF_RenderUTF8_Shaded(font, text, *text_colour, *background_colour);

	if (!surface) {
		return NULL;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(gfx->ren, surface);
	if (!texture) {
		printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
	}

	SDL_FreeSurface(surface);

	return texture;
}

void render_rects(graphics *gfx, generic_rect *rect, global_values *global) {

	if (!rect || !rect->common || !rect->data) {
		return;
	}

	input_specific *input = NULL;
	text_specific *text = NULL;
	button_specific *button = NULL;

	switch (rect->type) {
		case INPUT:
			input = (input_specific *)rect->data;
			break;
		case TEXT:
			text = (text_specific *)rect->data;
			break;
		case BUTTON:
			button = (button_specific *)rect->data;
			break;
		default:
			return;
			break;
	}

	if (check_state(global->needs_update, rect->common->id)) {
		if (rect->common->textures.text_texture) {
			SDL_DestroyTexture(rect->common->textures.text_texture);
			rect->common->textures.text_texture = NULL;
		}

		switch (rect->type) {
			case INPUT:
				if (strlen(input->input_text) == 0 && input->is_previewed) {
					rect->common->textures.text_texture = update_texture(gfx,
																													gfx->fonts[(input->preview_font >> 2) & 0x03][input->preview_font & 0x03],
																													input->preview_text,
																													&input->preview_colour,
																													&rect->common->colours.main_obj);

				} else {
					rect->common->textures.text_texture = update_texture(gfx,
																													gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03],
																													input->input_text,
																													&rect->common->colours.content_text,
																													&rect->common->colours.main_obj);
				}
				break;
			case TEXT:
				rect->common->textures.text_texture = update_texture(gfx, 
																												 gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03], 
																												 text->label,
																												 &rect->common->colours.content_text,
																												 &rect->common->colours.main_obj);

				break;
			case BUTTON:
				if (button->render_click) {
					rect->common->textures.text_texture = update_texture(gfx,
																													gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03],
																													button->label,
																													&button->on_click->content_text,
																													&button->on_click->main_obj);
				} else {
					rect->common->textures.text_texture = update_texture(gfx,
																													gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03],
																													button->label,
																													&rect->common->colours.content_text,
																													&rect->common->colours.main_obj);	
				}

				break;
			default:
				break;
		}

		if (rect->common->textures.text_texture) {

			int text_width, text_height;


			switch (rect->type) {
				case INPUT:
					if (input->is_previewed && strlen(input->input_text) == 0) {
						TTF_SizeText(gfx->fonts[(input->preview_font >> 2) & 0x03][input->preview_font & 0x03], input->preview_text, &text_width, &text_height);
					} else {
						TTF_SizeText(gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03], input->input_text, &text_width, &text_height);
					}
					break;
				case TEXT:
					TTF_SizeText(gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03], text->label, &text_width, &text_height);
					break;
				case BUTTON:
					TTF_SizeText(gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03], button->label, &text_width, &text_height);
					break;
				default:
					break;
			}

			rect->common->textures.text_rect.w = text_width;
			rect->common->textures.text_rect.h = text_height;

			if (check_state(global->is_centred, rect->common->id)) {
				rect->common->textures.text_rect.x = rect->common->boxes.main_box.x + 
					(rect->common->boxes.main_box.w - text_width) / 2;
			} else {
				rect->common->textures.text_rect.x = rect->common->boxes.main_box.x + 5;
			}

			rect->common->textures.text_rect.y = rect->common->boxes.main_box.y + 
				(rect->common->boxes.main_box.h - text_height) / 2;
		}

		clear_state(global->needs_update, rect->common->id);

	}

	switch (rect->type) {
		case INPUT:
			SDL_SetRenderDrawColor(gfx->ren,
													rect->common->colours.border_obj.r,
													rect->common->colours.border_obj.g,
													rect->common->colours.border_obj.b,
													rect->common->colours.border_obj.a);
			SDL_RenderFillRect(gfx->ren, &rect->common->boxes.border_box);

			SDL_SetRenderDrawColor(gfx->ren,
													rect->common->colours.main_obj.r,
													rect->common->colours.main_obj.g,
													rect->common->colours.main_obj.b,
													rect->common->colours.main_obj.a);
			SDL_RenderFillRect(gfx->ren, &rect->common->boxes.main_box);
			break;
		case BUTTON:
			if (button->render_click) {
				SDL_SetRenderDrawColor(gfx->ren,
													 button->on_click->border_obj.r,
													 button->on_click->border_obj.g,
													 button->on_click->border_obj.b,
													 button->on_click->border_obj.a);
				SDL_RenderFillRect(gfx->ren, &rect->common->boxes.border_box);
				SDL_SetRenderDrawColor(gfx->ren,
													 button->on_click->main_obj.r,
													 button->on_click->main_obj.g,
													 button->on_click->main_obj.b,
													 button->on_click->main_obj.a);
				SDL_RenderFillRect(gfx->ren, &rect->common->boxes.main_box);
			}
			else {
				SDL_SetRenderDrawColor(gfx->ren,
													 rect->common->colours.border_obj.r,
													 rect->common->colours.border_obj.g,
													 rect->common->colours.border_obj.b,
													 rect->common->colours.border_obj.a);
				SDL_RenderFillRect(gfx->ren, &rect->common->boxes.border_box);
				SDL_SetRenderDrawColor(gfx->ren,
													 rect->common->colours.main_obj.r,
													 rect->common->colours.main_obj.g,
													 rect->common->colours.main_obj.b,
													 rect->common->colours.main_obj.a);
				SDL_RenderFillRect(gfx->ren, &rect->common->boxes.main_box);
			}
			break;
		case TEXT:

			SDL_SetRenderDrawColor(gfx->ren,
													rect->common->colours.border_obj.r,
													rect->common->colours.border_obj.g,
													rect->common->colours.border_obj.b,
													rect->common->colours.border_obj.a);
			SDL_RenderFillRect(gfx->ren, &rect->common->boxes.border_box);

			SDL_SetRenderDrawColor(gfx->ren,
													rect->common->colours.main_obj.r,
													rect->common->colours.main_obj.g,
													rect->common->colours.main_obj.b,
													rect->common->colours.main_obj.a);
			SDL_RenderFillRect(gfx->ren, &rect->common->boxes.main_box);
			break;
		default:
			break;
	}

	SDL_RenderCopy(gfx->ren, rect->common->textures.text_texture, NULL, &rect->common->textures.text_rect);

	uint32_t current_time = SDL_GetTicks();
	static uint32_t old_time;

	switch (rect->type) {
		case INPUT:
			if (current_time - old_time >= 500) {
				old_time = SDL_GetTicks();
				global->render_cursor = !global->render_cursor;
			}
			break;
		case TEXT:
			break;
		case BUTTON:
			if (current_time - button->click_timer >= 200 && button->render_click) {
				button->render_click = SDL_FALSE;
				set_state(global->needs_update, rect->common->id);
			}
			break;
		default:
			break;
	}

	switch (rect->type) {
		case INPUT:
			if (global->render_cursor && input->is_active) {
				int text_width = font_text_width(
					gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03],
					input->input_text,
					global->cursor_pos
				);

				int cursor_x, cursor_y;
				int cursor_width = font_text_width(gfx->fonts[(rect->common->font >> 2) & 0x03][rect->common->font & 0x03], " ", 1);

				if (check_state(global->is_centred, rect->common->id)) {
					cursor_x = rect->common->textures.text_rect.x + text_width;
				} else {
					cursor_x = rect->common->boxes.main_box.x + 5 + text_width;
				}

				cursor_y = rect->common->textures.text_rect.y + rect->common->textures.text_rect.h;

				SDL_SetRenderDrawColor(gfx->ren, 0, 0, 0, 255);
				SDL_RenderDrawLine(gfx->ren, cursor_x, cursor_y, cursor_x + cursor_width, cursor_y);
			}
			break;
		case TEXT:
			break;
		case BUTTON:
			break;
		default:
			break;
	}

}

void render_objects(graphics *gfx, global_values *global, generic_rect **rects, int *rect_count) {

	SDL_SetRenderDrawColor(gfx->ren, 255, 255, 255, 255);
	SDL_RenderClear(gfx->ren);

	for (int i = 0; i < *rect_count; i++) {
		generic_rect *rect = rects[i];
		render_rects(gfx, rect, global);
	}

	SDL_RenderPresent(gfx->ren);
}

void check_events(graphics *gfx, keymap *keys, global_values *global, generic_rect **rects, int *rect_count) {
	while (SDL_PollEvent(&gfx->event)) {

		int x, y;

		switch (gfx->event.type) {
			case SDL_QUIT:
				keys->quit = 1;
				break;

			case SDL_KEYDOWN:
				switch (gfx->event.key.keysym.sym) {
					case SDLK_ESCAPE:
						keys->quit = 1;
						break;

					default:
						break;
				}

				if (global->active_input_rect) {
					input_specific *data = (input_specific *)global->active_input_rect->data;
					switch (gfx->event.key.keysym.sym) {
						case SDLK_BACKSPACE:
							if (global->cursor_pos > 0) {
								memmove(&data->input_text[global->cursor_pos - 1], &data->input_text[global->cursor_pos], strlen(data->input_text) - global->cursor_pos + 1);
								global->cursor_pos--;
								set_state(global->needs_update, global->active_input_rect->common->id);
							}
							break;

						case SDLK_LEFT:
							if (global->cursor_pos > 0) {
								global->cursor_pos--;
							}
							break;

						case SDLK_RIGHT:
							if (global->cursor_pos < strlen(data->input_text)) {
								global->cursor_pos++;
							}
							break;

						default:
							break;
					}
				}
				break;

			case SDL_TEXTINPUT:
				if (global->active_input_rect) {
					input_specific *data = (input_specific *)global->active_input_rect->data;

					size_t current_length = strlen(data->input_text);
					size_t new_size = current_length + strlen(gfx->event.text.text) + 1;

					char *new_buffer = realloc(data->input_text, new_size);
					if (!new_buffer) {
						fprintf(stderr, "Failed to allocate memory\n");
						exit(EXIT_FAILURE);
					}
					data->input_text = new_buffer;

					memmove(&data->input_text[global->cursor_pos + strlen(gfx->event.text.text)],
						 &data->input_text[global->cursor_pos],
						 strlen(data->input_text) - global->cursor_pos + 1);

					memcpy(&data->input_text[global->cursor_pos], gfx->event.text.text, strlen(gfx->event.text.text));
					global->cursor_pos += strlen(gfx->event.text.text);

					set_state(global->needs_update, global->active_input_rect->common->id);
				}
				break;

			case SDL_MOUSEBUTTONDOWN:

				SDL_GetMouseState(&x, &y);

				for (int i = 0; i < *rect_count; i++) {
					input_specific *input = NULL;
					text_specific *text = NULL;
					button_specific *button = NULL;

					input_specific *active_input = NULL;
					input_specific *child_input = NULL;

					switch (rects[i]->type) {
						case INPUT:
							input = (input_specific *)rects[i]->data;
							break;
						case TEXT:
							text = (text_specific *)rects[i]->data;
							break;
						case BUTTON:
							button = (button_specific *)rects[i]->data;
							break;
						default:
							continue;
					}

					if (SDL_PointInRect(&(SDL_Point){x, y}, &rects[i]->common->boxes.main_box)) {
						switch (rects[i]->type) {
							case INPUT:
								if (global->active_input_rect && global->active_input_rect != rects[i]) {
									input_specific *prev_input = (input_specific *)global->active_input_rect->data;
									prev_input->is_active = 0;
									prev_input->is_previewed = 1;
									set_state(global->needs_update, global->active_input_rect->common->id);
								}

								global->active_input_rect = rects[i];
								input = (input_specific *)rects[i]->data;

								input->is_active = 1;
								input->is_previewed = 0;
								global->render_cursor = 1;
								global->cursor_pos = strlen(input->input_text);
								set_state(global->needs_update, rects[i]->common->id);
								break;

							case TEXT:
								break;

							case BUTTON:
								if (!button->render_click) {
									button->render_click = SDL_TRUE;
									button->click_timer = SDL_GetTicks();
									set_state(global->needs_update, rects[i]->common->id);

									for (int j = 0; j < rects[i]->common->children_count; j++) {
										generic_rect *child = rects[i]->common->children[j];
										switch (child->type) {
											case INPUT:
												child_input = (input_specific *)child->data;
												printf("%s: %s\n", child_input->preview_text, child_input->input_text);
												memset(child_input->input_text, 0, strlen(child_input->input_text) + 1);
												global->cursor_pos = 0;
												set_state(global->needs_update, child->common->id);
												break;

											case TEXT:
												break;

											case BUTTON:
												break;

											default:
												break;
										}
									}
								}
								break;

							default:
								break;
						}
					}

					if (global->active_input_rect) {
						if (!SDL_PointInRect(&(SDL_Point){x, y}, &global->active_input_rect->common->boxes.main_box)) {
							active_input = (input_specific *)global->active_input_rect->data;

							active_input->is_active = 0;
							active_input->is_previewed = 1;
							global->render_cursor = 0;
							set_state(global->needs_update, global->active_input_rect->common->id);
						}
					}
				}
				break;

			default:
				break;
		}
	}
}

int main() {
	graphics gfx;

	initialise_graphics(&gfx, font_paths, font_sizes);


	global_values global;
	global.cursor_pos = 0;
	global.active_input_rect = NULL;

	memset(global.is_visible, 0, BITMAP_SIZE);
	memset(global.is_static, 0, BITMAP_SIZE);
	memset(global.needs_update, 0, BITMAP_SIZE);
	memset(global.is_centred, 0, BITMAP_SIZE);

	int rect_count = 3;
	generic_rect **rects = (generic_rect **)malloc(sizeof(generic_rect *) * rect_count);
	rects[0] = (generic_rect *)malloc(sizeof(generic_rect));
	rects[0]->common = (rect_common *)malloc(sizeof(rect_common));
	*(rects[0]->common) = (rect_common){
		.boxes = {
			.main_box = (SDL_Rect){66, 100, 434, 30},
			.border_box = (SDL_Rect){64, 98, 438, 34}
		},
		.colours = {
			.main_obj = (SDL_Colour){255, 255, 255, 255},
			.border_obj = (SDL_Colour){0, 0, 0, 255},
			.content_text = (SDL_Colour){0, 0, 0, 255},
		},
		.font = (FONT_REGULAR << 2) | FONT_SIZE_EXTRA_SMALL,
		.id = 0,
		.priority = 1,
		.children_count = 0,
	};

	{
		rects[0]->type = INPUT;
		rects[0]->data = malloc(sizeof(input_specific));
		input_specific *input = (input_specific *)rects[0]->data;

		input->input_text = malloc(strlen("") + 1);
		strcpy(input->input_text, "");

		input->preview_text = malloc(strlen("Username") + 1);
		strcpy(input->preview_text, "Username");

		input->is_active = 0;
		input->is_previewed = 1;
		input->preview_font = (FONT_ITALIC << 2) | FONT_SIZE_EXTRA_SMALL;
		input->preview_colour = (SDL_Colour){128, 128, 128, 255};
	}

	set_state(global.is_visible, rects[0]->common->id);
	set_state(global.needs_update, rects[0]->common->id);

	rects[2] = (generic_rect *)malloc(sizeof(generic_rect));
	rects[2]->common = (rect_common *)malloc(sizeof(rect_common));
	*(rects[2]->common) = (rect_common){
		.boxes = {
			.main_box = (SDL_Rect){66, 150, 434, 30},
			.border_box = (SDL_Rect){64, 148, 438, 34}
		},
		.colours = {
			.main_obj = (SDL_Colour){255, 255, 255, 255},
			.border_obj = (SDL_Colour){0, 0, 0, 255},
			.content_text = (SDL_Colour){0, 0, 0, 255},
		},
		.font = (FONT_REGULAR << 2) | FONT_SIZE_EXTRA_SMALL,
		.id = 2,
		.priority = 1,
		.children_count = 0,
	};

	{
		rects[2]->type = INPUT;
		rects[2]->data = malloc(sizeof(input_specific));
		input_specific *input = (input_specific *)rects[2]->data;

		input->input_text = malloc(strlen("") + 1);
		strcpy(input->input_text, "");

		input->preview_text = malloc(strlen("Password") + 1);
		strcpy(input->preview_text, "Password");

		input->is_active = 0;
		input->is_previewed = 1;
		input->preview_font = (FONT_ITALIC << 2) | FONT_SIZE_EXTRA_SMALL;
		input->preview_colour = (SDL_Colour){128, 128, 128, 255};

	}

	set_state(global.is_visible, rects[2]->common->id);
	set_state(global.needs_update, rects[2]->common->id);

	rects[1] = (generic_rect *)malloc(sizeof(generic_rect));
	rects[1]->common = (rect_common *)malloc(sizeof(rect_common));
	*(rects[1]->common) = (rect_common){
		.boxes = {
			.main_box = (SDL_Rect){66, 200, 60, 30},
			.border_box = (SDL_Rect){64, 198, 64, 34}
		},
		.colours = {
			.main_obj = (SDL_Colour){255, 255, 255, 255},
			.border_obj = (SDL_Colour){0, 0, 0, 255},
			.content_text = (SDL_Colour){0, 0, 0, 255},
		},
		.font = (FONT_REGULAR << 2) | FONT_SIZE_EXTRA_SMALL,
		.id = 1,
		.priority = 1,
	};

	{
		rects[1]->type = BUTTON;
		rects[1]->data = malloc(sizeof(button_specific));
		button_specific *button = (button_specific *)rects[1]->data;

		button->label = malloc(strlen("Submit") + 1);
		strcpy(button->label, "Submit");

		button->on_click = &(colour_list){
			.main_obj = (SDL_Colour){0, 0, 0, 255},
			.border_obj = (SDL_Colour){128, 128, 128, 255},
			.content_text = (SDL_Colour){255, 255, 255, 255},
		};

		button->render_click = SDL_FALSE;
		button->click_timer = 0;
	}

	set_state(global.is_visible, rects[1]->common->id);
	set_state(global.needs_update, rects[1]->common->id);

	rects[1]->common->children_count = 2;
	rects[1]->common->children = malloc(sizeof(generic_rect *) * rects[1]->common->children_count);

	rects[1]->common->children[0] = rects[0];
	rects[1]->common->children[1] = rects[2];

	keymap keys = {0};

	while (!keys.quit) {
		check_events(&gfx, &keys, &global, rects, &rect_count);
		render_objects(&gfx, &global, rects, &rect_count);
		SDL_FlushEvents(0, 0xFFFF);
	}

	exit_SDL(&gfx, rects, &rect_count);

	return 0;
}
