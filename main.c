#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <libgen.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "include/nfd.h"
#include "chip8.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 450
#define SCREEN_OFFSETX (WINDOW_WIDTH * 0.5 - SCREEN_SCALE * 64 * 0.5)
#define SCREEN_OFFSETY 20

char romDirectory[256] = ".";
int SCREEN_SCALE = 8;

typedef struct {
	uint32_t x;
	uint32_t y;
	char* text;
	TTF_Font* font;
	SDL_Color textColor;
	struct {
		uint8_t r, g, b, a;
	} color;
	bool pressed;
	bool mouseover;
} button_t;

void draw_rectangle(SDL_Renderer* renderer, int x, int y, int width, int height) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_RenderFillRect(renderer, &rect);
}

void render_screen(SDL_Renderer* renderer, bool padding) {
	int i = 0;
	int j = 0;
	uint8_t colors[4] = {0, 85, 170, 255};
	int ss = (SCREEN_SCALE / (extendedScreen+1));
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect screen_rect;
	screen_rect.x = SCREEN_OFFSETX * padding;
	screen_rect.y = SCREEN_OFFSETY * padding;
	screen_rect.w = SCREEN_SCALE * 64;
	screen_rect.h = SCREEN_SCALE * 32;
	SDL_RenderFillRect(renderer, &screen_rect);
	for(j = 0; j < 1; j++) {
		if(j == 0) SDL_SetRenderDrawColor(renderer, colors[3], colors[3], colors[3], 255);
		if(j == 1) SDL_SetRenderDrawColor(renderer, colors[1], colors[1], colors[1], 255);
		for(i = 0; i < pixel_number; i++) {
			if(canvas_data[j][i]) {
				if(j == 1 && canvas_data[j-1][i])
					SDL_SetRenderDrawColor(renderer, colors[2], colors[2], colors[2], 255);
				int x = (i % screen_width) * ss;
				int y = floor(i / screen_width) * ss;
				SDL_Rect rect;
				rect.x = x + SCREEN_OFFSETX * padding;
				rect.y = y + SCREEN_OFFSETY * padding;
				rect.w = ss;
				rect.h = ss;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

void render_toolbar(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 107, 150, 110, 255);
	draw_rectangle(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

bool chooseGame() {
	nfdchar_t* outpath = NULL;
	nfdresult_t result = NFD_OpenDialog(NULL, romDirectory, &outpath);
	char pathcpy[256];
	strcpy(pathcpy, outpath);
	strcpy(romDirectory, dirname(pathcpy));
	
	if(result == NFD_OKAY) {
		loadProgram(outpath);
		return true;
	} else if(result = NFD_CANCEL) {
		return false;
	} else {
		printf("Error: %s\n", NFD_GetError());
		return false;
	}
}

void printText(SDL_Renderer* renderer, TTF_Font* font, char* text, int x, int y, SDL_Color color) {
	SDL_Surface* text_surface;
	if(!(text_surface = TTF_RenderText_Solid(font, text, color))) {
		//printf("Failed to render text.");
	} else {
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		SDL_QueryTexture(text_texture, NULL, NULL, &(srcRect.w), &(srcRect.h));
		SDL_Rect dstRect;
		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = srcRect.w;
		dstRect.h = srcRect.h;
		SDL_RenderCopy(renderer, text_texture, &srcRect, &dstRect);
		SDL_DestroyTexture(text_texture);
	}
}

void button_mouse_event(button_t* button, const SDL_Event* ev) {
	int w, h;
	TTF_SizeText(button->font, button->text, &w, &h);
	w += 40;
	h += 20;
	if(ev->type == SDL_MOUSEBUTTONDOWN) {
		if(ev->button.button == SDL_BUTTON_LEFT && ev->button.x >= button->x && ev->button.x <= button->x + w && ev->button.y >= button->y && ev->button.y <= button->y + h) {
			button->pressed = true;
		}
	}
}

bool button(SDL_Renderer* renderer, button_t* button) {
	int w, h;
	TTF_SizeText(button->font, button->text, &w, &h);
	w += 40;
	h += 20;
	SDL_Rect rect;
	rect.x = button->x;
	rect.y = button->y;
	rect.w = w;
	rect.h = h;
	int colorAdd = 0;
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	if(mx >= button->x && mx <= button->x + w && my >= button->y && my <= button->y + h) {
		button->mouseover = true;
	} else {
		button->mouseover = false;
	}
	if(button->mouseover) {
		colorAdd = 20;
	}
	SDL_SetRenderDrawColor(renderer, button->color.r + colorAdd, button->color.g + colorAdd, button->color.b + colorAdd, button->color.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, button->color.a);
	SDL_RenderDrawRect(renderer, &rect);
	printText(renderer, button->font, button->text, button->x + 20, button->y + 10, button->textColor);
	
	if(button->pressed) {
		button->pressed = false;
		return true;
	}
	return false;
}

char* sanitizeCpf(char* input) {
	char* output;
	output = (char*)malloc(sizeof(char));
	uint8_t length = strlen(input);
	strcpy(output, "");
	for(int i = 0; i < length; i++) {
		if(isdigit(input[i])) {
			uint8_t len = strlen(output);
			char buffer[len + 2];
			strcpy(buffer, output);
			buffer[len] = input[i];
			buffer[len + 1] = '\0';
			strcpy(output, buffer);
		}
	}
	return output;
}

int main(int argc, char* argv[]) {
	int cpf = 100;
	bool nogui = false;
	char openFile[256] = "null";
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			printf("\nUsage:\n\n\t-h, --help\t\tDisplay available command line arguments.\n");
			printf("\t-n, --nogui\t\tStart Chip-84 without UI elements, leaving only the display shown.\n");
			printf("\t-o, --open [path]\tDirectly load a ROM upon launch.\n");
			printf("\t-c, --cpf [number]\tStart Chip-84 with an initial cycles per frame value.\n");
			printf("\t-s, --screenscale [number]\tSet the scale of the display.\n");
			return 1;
		}
		if(strcmp(argv[i], "--nogui") == 0 || strcmp(argv[i], "-n") == 0) {
			nogui = true;
		}
		if(strcmp(argv[i], "--open") == 0 || strcmp(argv[i], "-o") == 0) {
			if(i+1<argc)
				strcpy(openFile, argv[i+1]);
		}
		if(strcmp(argv[i], "--cpf") == 0 || strcmp(argv[i], "-c") == 0) {
			if(i+1<argc)
				cpf = atoi(sanitizeCpf(argv[i+1]));
		}
		if(strcmp(argv[i], "--screenscale") == 0 || strcmp(argv[i], "-s") == 0) {
			if(i+1<argc)
				SCREEN_SCALE = atoi(sanitizeCpf(argv[i+1]));
		}
	}

	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	if(nogui)
		window = SDL_CreateWindow("Chip-84 SDL Edition --nogui", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SCALE*64, SCREEN_SCALE*32, SDL_WINDOW_SHOWN);
	else
		window = SDL_CreateWindow("Chip-84 SDL Edition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	
	if(window == NULL) {
		printf("Error creating window: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL) {
		SDL_DestroyWindow(window);
		printf("Error creating renderer: %s\n", SDL_GetError());
	}

	if(TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return 1;
	}

	TTF_Font* font;
	font = TTF_OpenFont("DOS.ttf", 16);
	if(!font) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
	}

	button_t openButton = {
		.x = 50,
		.y = SCREEN_SCALE * 32 + SCREEN_OFFSETY + 50,
		.text = "Open ROM...",
		.color = { .r=200, .g=200, .b=200, .a=255 },
		.font = font,
	};
	button_t cpfButton = {
		.x = 50,
		.y = SCREEN_SCALE * 32 + SCREEN_OFFSETY + 100,
		.text = "Edit Cycles Per Frame...",
		.color = { .r=200, .g=200, .b=200, .a=255 },
		.font = font,
	};

	if(strcmp(openFile, "null") != 0) {
		loadProgram(openFile);
	}
	
	SDL_Event e;
	int quit = 0;
	bool ctrl = false;
	char* textInput;

	textInput = (char*)malloc(1*sizeof(char));
	SDL_StopTextInput();
	while(!quit) {

		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym) {
						case SDLK_1:
							keys[0x1] = 1;
							break;
						case SDLK_2:
							keys[0x2] = 1;
							break;
						case SDLK_3:
							keys[0x3] = 1;
							break;
						case SDLK_4:
							keys[0xc] = 1;
							break;
						case SDLK_q:
							keys[0x4] = 1;
							break;
						case SDLK_w:
							keys[0x5] = 1;
							break;
						case SDLK_e:
							keys[0x6] = 1;
							break;
						case SDLK_r:
							keys[0xd] = 1;
							break;
						case SDLK_a:
							keys[0x7] = 1;
							break;
						case SDLK_s:
							keys[0x8] = 1;
							break;
						case SDLK_d:
							keys[0x9] = 1;
							break;
						case SDLK_f:
							keys[0xe] = 1;
							break;
						case SDLK_z:
							keys[0xa] = 1;
							break;
						case SDLK_x:
							keys[0x0] = 1;
							break;
						case SDLK_c:
							keys[0xb] = 1;
							break;
						case SDLK_v:
							keys[0xf] = 1;
							break;
						case SDLK_LCTRL:
							ctrl = true;
							break;
						case SDLK_RCTRL:
							ctrl = true;
							break;
						case SDLK_o:
							if(ctrl)
								chooseGame();
							break;
						case SDLK_BACKSPACE:
							textInput[strlen(textInput)-1] = '\0';
							break;
						case SDLK_RETURN:
							SDL_StopTextInput();
							cpfButton.text = "Edit Cycles Per Frame...";
							cpf = atoi(textInput);
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					switch(e.key.keysym.sym) {
						case SDLK_1:
							keys[0x1] = 0;
							break;
						case SDLK_2:
							keys[0x2] = 0;
							break;
						case SDLK_3:
							keys[0x3] = 0;
							break;
						case SDLK_4:
							keys[0xc] = 0;
							break;
						case SDLK_q:
							keys[0x4] = 0;
							break;
						case SDLK_w:
							keys[0x5] = 0;
							break;
						case SDLK_e:
							keys[0x6] = 0;
							break;
						case SDLK_r:
							keys[0xd] = 0;
							break;
						case SDLK_a:
							keys[0x7] = 0;
							break;
						case SDLK_s:
							keys[0x8] = 0;
							break;
						case SDLK_d:
							keys[0x9] = 0;
							break;
						case SDLK_f:
							keys[0xe] = 0;
							break;
						case SDLK_z:
							keys[0xa] = 0;
							break;
						case SDLK_x:
							keys[0x0] = 0;
							break;
						case SDLK_c:
							keys[0xb] = 0;
							break;
						case SDLK_v:
							keys[0xf] = 0;
							break;
						case SDLK_LCTRL:
							ctrl = false;
							break;
						case SDLK_RCTRL:
							ctrl = false;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_TEXTINPUT:
					strcat(textInput, e.text.text);
					strcpy(textInput, sanitizeCpf(textInput));
					break;
			}

			if(!nogui) {
				button_mouse_event(&openButton, &e);
				button_mouse_event(&cpfButton, &e);
			}
		}
		
		emulateCycle(cpf);
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		if(!nogui)
			render_toolbar(renderer);
		render_screen(renderer, !nogui);

		if(!nogui) {
			SDL_Color black = { 0, 0, 0 };
			
			
			if(button(renderer, &openButton)) {
				chooseGame();
			}
			if(button(renderer, &cpfButton)) {
				if(cpfButton.text == "          Done          ") {
					SDL_StopTextInput();
					cpfButton.text = "Edit Cycles Per Frame...";
					cpf = atoi(textInput);
				} else {
					char cpfString[64];
					sprintf(cpfString, "%d", cpf);
					strcpy(textInput, cpfString);
					SDL_StartTextInput();
					cpfButton.text = "          Done          ";
				}
			}

			char cpfString[64];
			if(cpfButton.text == "          Done          ") {
				sprintf(cpfString, "%s_", textInput);
			} else {
				sprintf(cpfString, "%d", cpf);
			}
			printText(renderer, font, cpfString, 325, SCREEN_SCALE * 32 + SCREEN_OFFSETY + 110, black);
		}

		SDL_RenderPresent(renderer);
	}
	
	TTF_CloseFont(font);
	font = NULL;
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
