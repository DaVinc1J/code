#include "ui.h"
//#include "global.h"
//#include "graphics.h"

int main() {
	asset_count box_count = {0};
	assets boxes = read_assets("assets.ass", &box_count);

	printf("Number of input boxes: %d\n", box_count.input_box_count);
	printf("Number of text boxes: %d\n", box_count.text_box_count);
	printf("Number of button boxes: %d\n", box_count.button_box_count);

	free(boxes.input_boxes);
	free(boxes.text_boxes);
	free(boxes.button_boxes);

	return 0;
}
