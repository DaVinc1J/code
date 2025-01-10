#ifndef DEFUNCT_H
#define DEFUNCT_H

#include "ui.h"

typedef struct {
	rect_common common;
	input_specific data;	
} input_rect;

typedef struct {
	rect_common common;
	text_specific data;
} text_rect;

typedef struct {
	rect_common common;
	button_specific data;
} button_rect;

typedef struct {
	text_rect *text_boxes;
	input_rect *input_boxes;
	button_rect *button_boxes;
} assets;

typedef struct {
	int text_box_count;
	int input_box_count;
	int button_box_count;
} asset_count;

#endif
