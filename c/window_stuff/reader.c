#include "global.h"
//#include "graphics.h"
#include "ui.h"

void read_to_shared_rect(long byte, FILE *file, void *src) {
	text_rect *rect = (text_rect *)src;

	char buffer[256];
	
	int border = 0;

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[boxes]", 7) != 0) {return;}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		
		border = 10;
		rect->boxes.main_box = (SDL_Rect){100, 100, 200, 200};
		rect->boxes.border_box = (SDL_Rect){95, 95, 210, 210};
		rect->colours.main_colour = (SDL_Colour){255, 255, 255, 255};
		rect->colours.border_colour = (SDL_Colour){0, 0, 0, 255};

	} else {
		sscanf(buffer, "size::{%d,%d}", 
				 &rect->boxes.main_box.w, 
				 &rect->boxes.main_box.h);

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "pos::{%d,%d}", 
				 &rect->boxes.main_box.x, 
				 &rect->boxes.main_box.y);

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "border::{%d}", &border);

		if (border != 0) {
			rect->boxes.border_box.w = 
				rect->boxes.main_box.w + border;
			rect->boxes.border_box.h =
				rect->boxes.main_box.h + border;
			rect->boxes.border_box.x = 
				rect->boxes.main_box.x - (border / 2);
			rect->boxes.border_box.y = 
				rect->boxes.main_box.y - (border / 2);
		};

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "main_colour::{%hhu,%hhu,%hhu,%hhu}", 
				 &rect->colours.main_colour.r, 
				 &rect->colours.main_colour.g, 
				 &rect->colours.main_colour.b, 
				 &rect->colours.main_colour.a);

		fgets(buffer, sizeof(buffer), file);
		if (border != 0) {
			sscanf(buffer, "border_colour::{%hhu,%hhu,%hhu,%hhu}", 
					&rect->colours.border_colour.r, 
					&rect->colours.border_colour.g, 
					&rect->colours.border_colour.b, 
					&rect->colours.border_colour.a);
		}

	} 

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[options]", 9) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		rect->scale = 1.0f;
		rect->visible = 1;
		rect->is_static = 1;
		rect->id = -1;
		rect->priority = -1;
	} else {

		sscanf(buffer, "scale::{%f}", &rect->scale);

		int temp_visible, temp_is_static;
		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "visible::{%d}", &temp_visible);
		rect->visible = temp_visible;
		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "is_static::{%d}", &temp_is_static);
		rect->is_static = temp_is_static;

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "id::{%d}", &rect->id);

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "priority::{%d}", &rect->priority);

	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[text]", 6) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		rect->font_marker = 1;
		rect->colours.text_colour = (SDL_Colour){0, 0, 0, 255};
	} else {

		sscanf(buffer, "font::{%d}", &rect->font_marker);

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "text_colour::{%hhu,%hhu, %hhu,%hhu}",
				 &rect->colours.text_colour.r,
				 &rect->colours.text_colour.g,
				 &rect->colours.text_colour.b,
				 &rect->colours.text_colour.a);

	}
	
}

void read_to_button_rect(long byte, FILE *file, button_rect *rect) {
	char buffer[256];

	int count = 0;

	fseek(file, byte, SEEK_SET);

	fgets(buffer, sizeof(buffer), file);
	read_to_shared_rect(byte, file, rect);

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[input]", 7) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		
		rect->label[0] = '\0';
	
	} else {
		
		sscanf(buffer, "label::{%16[^}]}", rect->label);
	
	}
}

void read_to_text_rect(long byte, FILE *file, text_rect *rect) {
	char buffer[256];

	int count = 0;

	fseek(file, byte, SEEK_SET);

	fgets(buffer, sizeof(buffer), file);
	read_to_shared_rect(byte, file, rect);

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[label]", 7) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		
		rect->label[0] = '\0';
	
	} else {
	
		sscanf(buffer, "label::{%16[^}]}", rect->label);
	
	}
}

void read_to_input_rect(long byte, FILE *file, input_rect *rect) {
	char buffer[256];

	int count = 0;

	fseek(file, byte, SEEK_SET);

	fgets(buffer, sizeof(buffer), file);
	read_to_shared_rect(byte, file, rect);

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[input]", 7) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		
		rect->input.input_text[0] = '\0';
	
	} else {
		
		sscanf(buffer, "input_text::{%16[^}]}", rect->input.input_text);
	
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "[cursor options]", 16) != 0) {
		return;
	}

	fgets(buffer, sizeof(buffer), file);
	if (strncmp(buffer, "standard::{}", 12) == 0) {
		rect->input.cursor_pos = 0;
		rect->input.render_cursor = 0;
		rect->input.cursor_timer = 0;
		rect->input.active = 0;
	} else {

		sscanf(buffer, "cursor_pos::{%d}", &rect->input.cursor_pos);

		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "cursor_timer::{%d}", &rect->input.cursor_timer);

		fgets(buffer, sizeof(buffer), file);
		int temp_render_cursor, temp_active;
		sscanf(buffer, "render_cursor::{%d}", &temp_render_cursor);
		rect->input.render_cursor = temp_render_cursor;
		fgets(buffer, sizeof(buffer), file);
		sscanf(buffer, "active::{%d}", &temp_active);
		rect->input.active = temp_active;

	}
}

assets read_assets(char *filename, asset_count *box_count) {
	assets boxes = {0};
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("Error: Could not open file %s\n", filename);
    return boxes;
	}

	char buffer[256];
	long positions[256];
	
	int position_count = 0;
	int count = 0;
	
	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		count++;
		long pos = ftell(file);

		if (strncmp(buffer, "[type]", 6) == 0) {
			fgets(buffer, sizeof(buffer), file);
			count++;
			if (strncmp(buffer, "rect::{input_rect}", 18) == 0) {
				box_count->input_box_count++;
				positions[position_count++] = pos;
			} else if (strncmp(buffer, "rect::{text_rect}", 17) == 0) {
				box_count->text_box_count++;
				positions[position_count++] = pos;
			} else if (strncmp(buffer, "rect::{button_rect}", 19) == 0) {
				box_count->button_box_count++;
				positions[position_count++] = pos;
			}
		}
	}
	boxes.input_boxes = malloc(box_count->input_box_count * sizeof(input_rect));
	boxes.text_boxes = malloc(box_count->text_box_count * sizeof(text_rect));
	boxes.button_boxes = malloc(box_count->button_box_count * sizeof(button_rect));

	rewind(file);

	int input_index = 0;
	int text_index = 0;
	int button_index = 0;

	for (int i = 0; i < position_count; i++) {
		fseek(file, positions[i], SEEK_SET);
		fgets(buffer, sizeof(buffer), file);

		if (strncmp(buffer, "rect::{input_rect}", 18) == 0) {
			read_to_input_rect(positions[i], file, &boxes.input_boxes[input_index]);
			input_index++;
		}

		if (strncmp(buffer, "rect::{text_rect}", 17) == 0) {
			read_to_text_rect(positions[i], file, &boxes.text_boxes[text_index]);
			text_index++;
		}

		if (strncmp(buffer, "rect::{button_rect}", 19) == 0) {
			read_to_button_rect(positions[i], file, &boxes.button_boxes[button_index]);
			button_index++;
		}
	}

	fclose(file);
	return boxes;
}
