#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 400
#define WIDTH 400
#define MAX_INPUT_CHARS 16 + 2
#define MAX_BOXES 7 // Maximum number of boxes

typedef struct {
	int quit;
} keymap;

typedef struct {
	TTF_Font **font;
	int numFonts;
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Event event;
} graphics;

typedef struct {
	char inputText[MAX_INPUT_CHARS];
	int cursorPos;
	SDL_bool renderCursor;
	Uint32 cursorTimer;
	SDL_bool active; // To track if this text input is active (has focus)
} textInput;

typedef struct {
	SDL_Rect shape;
	SDL_Color color;
	SDL_Color borderColor;
	int borderWidth;
	SDL_Color textColor;
} SDL;

typedef struct box {
	SDL_Rect shape;        // Position and size of the box
	SDL_Color color;       // Background color of the box
	SDL_Color borderColor; // Border color
	int borderWidth;       // Border width
	SDL_Color textColor;   // Text color
	TTF_Font *font;        // Font for text rendering
	TTF_Font *previewFont;
	char *previewText;
	SDL_Color previewColor;
	float scale;           // Scale factor for text rendering
	textInput *textInput; // Pointer to textInput struct if this box has text input
	char *label;    // Label text if any
	struct box **children;
	int childrenCount;
	int button;
	int visible;
	int preview;
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
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	TTF_Init();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	SDL_StartTextInput();

	gfx->font = malloc(sizeof(TTF_Font *) * 4);

	gfx->font[0] = TTF_OpenFont(
		"/Users/tristanlowe/nvim/fonts/AnonymiceProNerdFont-Bold.ttf",
		55);


	gfx->font[1] = TTF_OpenFont(
		"/Users/tristanlowe/nvim/fonts/AnonymiceProNerdFont-BoldItalic.ttf",
		55);


	gfx->font[2] = TTF_OpenFont(
		"/Users/tristanlowe/nvim/fonts/AnonymiceProNerdFont-Italic.ttf",
		55);


	gfx->font[3] = TTF_OpenFont(
		"/Users/tristanlowe/nvim/fonts/AnonymiceProNerdFont-Regular.ttf",
		55);

	gfx->numFonts = 4;

	gfx->win = SDL_CreateWindow(
		"", /* Name of window */
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, /* Where the window is positioned on screen */
		WIDTH, HEIGHT,          /* Window size */
		SDL_WINDOW_RESIZABLE);  /* Makes the window resizable */

	gfx->ren = SDL_CreateRenderer(
		gfx->win, /* Window where the renderer is displayed */
		-1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_PRESENTVSYNC); /* Hardware acceleration + synchronized
																				 refresh */
}

void checkEvents(graphics *gfx, keymap *keys, box boxes[], int boxCount) {

	while (SDL_PollEvent(
		&gfx->event)) { /* If an event is happening, i.e., key presses */

		if (gfx->event.type == SDL_QUIT) {
			keys->quit = 1; /* If you click exit it will exit the app */
		} else if (gfx->event.type == SDL_KEYDOWN) {
			switch (gfx->event.key.keysym.sym) {
				case SDLK_ESCAPE:
					keys->quit = 1; /* If you press escape it will exit the app */
					break;
				default:
					break;
			}

			// Handle text input navigation and backspace for the active box
			for (int i = 0; i < boxCount; i++) {
				textInput *ti = boxes[i].textInput;
				if (ti && ti->active) {
					switch (gfx->event.key.keysym.sym) {
						case SDLK_BACKSPACE:
							if (ti->cursorPos > 0) {
								memmove(&ti->inputText[ti->cursorPos - 1],
								&ti->inputText[ti->cursorPos],
								strlen(ti->inputText) - ti->cursorPos + 1);
								ti->cursorPos--;
							}
							break;
						case SDLK_RETURN:
							printf("User Input: %s\n", ti->inputText);
							memset(ti->inputText, 0, sizeof(ti->inputText));
							ti->inputText[0] = ' ';
							ti->cursorPos = 0;
							break;
						case SDLK_LEFT:
							if (ti->cursorPos > 0) {
								ti->cursorPos--;
							}
							break;
						case SDLK_RIGHT:
							if (ti->cursorPos < strlen(ti->inputText)) {
								ti->cursorPos++;
							}
							break;
						default:
							break;
					}
				}
			}
		} else if (gfx->event.type == SDL_TEXTINPUT) {
			// Handle text input for the active box
			for (int i = 0; i < boxCount; i++) {
				textInput *ti = boxes[i].textInput;
				if (ti && ti->active) {
					if (strlen(ti->inputText) + strlen(gfx->event.text.text) <
						MAX_INPUT_CHARS) {
						memmove(
							&ti->inputText[ti->cursorPos + strlen(gfx->event.text.text)],
							&ti->inputText[ti->cursorPos],
							strlen(ti->inputText) - ti->cursorPos + 1);
						memcpy(&ti->inputText[ti->cursorPos], gfx->event.text.text,
						 strlen(gfx->event.text.text));
						ti->cursorPos += strlen(gfx->event.text.text);
					}
				}
			}
		} else if (gfx->event.type == SDL_MOUSEBUTTONDOWN) {
			int x, y;
			SDL_GetMouseState(&x, &y);

			for (int i = 0; i < boxCount; i++) {
				textInput *ti = boxes[i].textInput;

				if (ti) {
					if (SDL_PointInRect(&(SDL_Point){x, y}, &boxes[i].shape)) {
						ti->active = SDL_TRUE;
					} else {
						ti->active = SDL_FALSE;
					}
				}

				if (boxes[i].button) {
					if (SDL_PointInRect(&(SDL_Point){x, y}, &boxes[i].shape)) {
						for (int j = 0; j < boxes[i].childrenCount; j++) {
							box *child = boxes[i].children[j];
							if (child && child->textInput) {
								textInput *childti = child->textInput;
								printf("User Input: %s\n", childti->inputText);
								memset(childti->inputText, 0, sizeof(childti->inputText));
								childti->inputText[0] = ' ';
								childti->cursorPos = 0;
							}
						}
					}
				}
			}
		}
	}
}

void renderObjects(graphics *gfx, box boxes[], int boxCount) {

	SDL_SetRenderDrawColor(gfx->ren,      /* Set drawing color */
												255, 255, 255, /* RGB values for white background */
												255);          /* Alpha value */

	SDL_RenderClear(gfx->ren);

	for (int i = 0; i < boxCount; i++) {
		box *b = &boxes[i];

		// Draw box background
		SDL_SetRenderDrawColor(gfx->ren, b->color.r, b->color.g, b->color.b,
												 b->color.a);
		SDL_RenderFillRect(gfx->ren, &b->shape);

		// Draw box border
		if (b->borderWidth > 0 && b->visible) {
			SDL_SetRenderDrawColor(gfx->ren, b->borderColor.r, b->borderColor.g,
													b->borderColor.b, b->borderColor.a);
			for (int w = 0; w < b->borderWidth; w++) {
				SDL_Rect borderRect = {b->shape.x - w, b->shape.y - w,
					b->shape.w + 2 * w, b->shape.h + 2 * w};
				SDL_RenderDrawRect(gfx->ren, &borderRect);
			}
		}

		// If the box has text input, render the text
		if (b->textInput && b->visible) {
			textInput *ti = b->textInput;

			SDL_Color textColor = b->textColor;

			// Render the text input
			char displayText[MAX_INPUT_CHARS];
			if (ti->inputText[0] == ' ' && ti->inputText[1] == '\0') {
				strncpy(displayText, b->previewText, MAX_INPUT_CHARS);
			} else {
				strncpy(displayText, ti->inputText, MAX_INPUT_CHARS);
			}
			displayText[MAX_INPUT_CHARS - 1] = '\0';

			SDL_Surface *textSurface = NULL;	

			if (ti->inputText[0] == ' ' && ti->inputText[1] == '\0') {
				textSurface = TTF_RenderUTF8_Blended(b->previewFont, displayText, b->previewColor);

			} else {
				textSurface = TTF_RenderUTF8_Blended(b->font, displayText, textColor);
			}

			SDL_Texture *textTexture =
				SDL_CreateTextureFromSurface(gfx->ren, textSurface);

			SDL_Rect srcRect = {0, 0, textSurface->w, textSurface->h};

			int scaledTextWidth = (int)(textSurface->w / b->scale);
			int scaledTextHeight = (int)(textSurface->h / b->scale);

			SDL_Rect textRect = {b->shape.x + 5, // Small padding
				b->shape.y + (b->shape.h - scaledTextHeight) / 2,
				scaledTextWidth, scaledTextHeight};

			SDL_RenderCopyEx(gfx->ren, textTexture, &srcRect, &textRect, 0, NULL,
										SDL_FLIP_NONE);

			// Handle cursor blinking
			Uint32 currentTime = SDL_GetTicks();
			if (currentTime - ti->cursorTimer >= 500) {
				ti->renderCursor = !ti->renderCursor;
				ti->cursorTimer = currentTime;
			}

			if (ti->active && ti->renderCursor) {
				int textWidth = fontTextWidth(b->font, ti->inputText, ti->cursorPos);
				int cursorX = textRect.x + (int)(textWidth / b->scale);
				SDL_SetRenderDrawColor(gfx->ren, 0, 0, 0, 255);
				SDL_RenderDrawLine(gfx->ren, cursorX, textRect.y, cursorX,
											 textRect.y + textRect.h);
			}

			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);
		}

		// If the box has a label (non-editable text), render it
		if (b->label && b->visible) {
			SDL_Color textColor = b->textColor;

			SDL_Surface *textSurface =
				TTF_RenderUTF8_Blended(b->font, b->label, textColor);

			SDL_Texture *textTexture =
				SDL_CreateTextureFromSurface(gfx->ren, textSurface);

			SDL_Rect srcRect = {0, 0, textSurface->w, textSurface->h};

			int scaledTextWidth = (int)(textSurface->w / b->scale);
			int scaledTextHeight = (int)(textSurface->h / b->scale);

			SDL_Rect textRect = {b->shape.x + (b->shape.w - scaledTextWidth) / 2,
				b->shape.y + (b->shape.h - scaledTextHeight) / 2,
				scaledTextWidth, scaledTextHeight};

			SDL_RenderCopyEx(gfx->ren, textTexture, &srcRect, &textRect, 0, NULL,
										SDL_FLIP_NONE);

			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);
		}
	}

	SDL_RenderPresent(gfx->ren);
}

void exitSDL(graphics *gfx) {
	SDL_StopTextInput();

	for (int i = 0; i < gfx->numFonts; i++) {
		TTF_CloseFont(gfx->font[i]);
	}
	gfx->font = NULL;

	SDL_DestroyRenderer(gfx->ren);
	gfx->ren = NULL;

	SDL_DestroyWindow(gfx->win);
	gfx->win = NULL;

	TTF_Quit();
	SDL_Quit();
}

int main() {

	graphics gfx; /* Initialize graphics struct */

	keymap keys = {0};

	initialise(&gfx); /* Initialize graphics */

	// Create an array of boxes
	static box boxes[MAX_BOXES];
	int boxCount = 0;

	// Initialize text inputs
	textInput textInputs[2];
	for (int i = 0; i < 2; i++) {

		textInputs[i].inputText[0] = ' ';
		textInputs[i].cursorPos = 0;
		textInputs[i].renderCursor = SDL_TRUE;
		textInputs[i].cursorTimer = SDL_GetTicks();
		textInputs[i].active = SDL_FALSE;
	}

	// Initialize boxes
	// input boxes
	boxes[0].shape = (SDL_Rect){(WIDTH - 250) / 2, 135, 250, 50};
	boxes[0].color = (SDL_Color){200, 200, 200, 255};
	boxes[0].borderColor = (SDL_Color){180, 180, 180, 255};
	boxes[0].borderWidth = 2;
	boxes[0].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[0].font = gfx.font[3];
	boxes[0].previewFont = gfx.font[2];
	boxes[0].previewText = "Username";
	boxes[0].previewColor = (SDL_Color){128, 128, 128, 255};
	boxes[0].scale = 2.0f;
	boxes[0].textInput = &textInputs[0];
	boxes[0].visible = 1;
	boxes[0].preview = 1;
	boxCount++;

	boxes[6].shape = (SDL_Rect){(WIDTH - 250) / 2, 205, 250, 50};
	boxes[6].color = (SDL_Color){200, 200, 200, 255};
	boxes[6].borderColor = (SDL_Color){180, 180, 180, 255};
	boxes[6].borderWidth = 2;
	boxes[6].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[6].font = gfx.font[3];
	boxes[6].previewFont = gfx.font[2];
	boxes[6].previewText = "Password";
	boxes[6].previewColor = (SDL_Color){128, 128, 128, 255};
	boxes[6].scale = 2.0f;
	boxes[6].textInput = &textInputs[1];
	boxes[6].visible = 1;
	boxes[6].preview = 1;
	boxCount++;

	// colour boxes
	boxes[1].shape = (SDL_Rect){0, 0, 400, 60};
	boxes[1].color = (SDL_Color){23, 147, 209, 255};
	boxes[1].borderColor = (SDL_Color){23, 147, 209, 255};
	boxes[1].borderWidth = 1;
	boxes[1].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[1].font = gfx.font[3];
	boxes[1].scale = 2.0f;
	boxes[1].visible = 1;
	boxCount++;

	boxes[2].shape = (SDL_Rect){0, 56, 400, 4};
	boxes[2].color = (SDL_Color){51, 51, 51, 255};
	boxes[2].borderColor = (SDL_Color){51, 51, 51, 255};
	boxes[2].borderWidth = 1;
	boxes[2].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[2].font = gfx.font[3];
	boxes[2].scale = 2.0f;
	boxes[2].visible = 1;
	boxCount++;

	//button box
	boxes[3].shape = (SDL_Rect){(WIDTH - 250) / 2, 275, 250, 50};
	boxes[3].color = (SDL_Color){100, 100, 100, 255};
	boxes[3].borderColor = (SDL_Color){51, 51, 51, 255};
	boxes[3].borderWidth = 2;
	boxes[3].textColor = (SDL_Color){255, 255, 255, 255};
	boxes[3].font = gfx.font[3];
	boxes[3].scale = 2.0f;
	boxes[3].label = "Submit";
	boxes[3].children = malloc(sizeof(box *) * 2);
	boxes[3].childrenCount = 2;
	boxes[3].button = 1;
	boxes[3].visible = 1;
	boxCount++;

	// text box
	boxes[4].shape = (SDL_Rect){(WIDTH - 100) / 2, 80, 100, 40};
	boxes[4].color = (SDL_Color){255, 255, 255, 255};
	boxes[4].borderColor = (SDL_Color){255, 255, 255, 255};
	boxes[4].borderWidth = 1;
	boxes[4].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[4].font = gfx.font[3];
	boxes[4].scale = 2.0f;
	boxes[4].label = "Script Key Login";
	boxes[4].visible = 1;
	boxCount++;

	boxes[5].shape = (SDL_Rect){55, 380, 40, 20};
	boxes[5].color = (SDL_Color){255, 255, 255, 255};
	boxes[5].borderColor = (SDL_Color){255, 255, 255, 255};
	boxes[5].borderWidth = 1;
	boxes[5].textColor = (SDL_Color){0, 0, 0, 255};
	boxes[5].font = gfx.font[3];
	boxes[5].scale = 3.0f;
	boxes[5].label = "Version: 0.0.0";
	boxes[5].visible = 1;
	boxCount++;

	boxes[3].children[0] = (struct box *)&boxes[0];
	boxes[3].children[1] = (struct box *)&boxes[6];

	while (!keys.quit) {
		checkEvents(&gfx, &keys, boxes, boxCount);
		renderObjects(&gfx, boxes, boxCount);
	}

	exitSDL(&gfx);

	return 0;
}
