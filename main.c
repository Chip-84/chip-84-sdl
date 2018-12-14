#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "chip8.h"

#define SCREEN_SCALE 6

void render_screen(SDL_Renderer *renderer) {
	int i = 0;
	int j = 0;
	uint8_t colors[4] = {0, 85, 170, 255};
	int ss = (SCREEN_SCALE / (extendedScreen+1));
	for(j = 0; j < 1; j++) {
		if(j == 0) SDL_SetRenderDrawColor(renderer, colors[3], colors[3], colors[3], 255);
		if(j == 1) SDL_SetRenderDrawColor(renderer, colors[1], colors[1], colors[1], 255);
		for(i = 0; i < pixel_number; i++) {
			if(canvas_data[j][i]) {
				if(j == 1 && canvas_data[j-1][i])
					SDL_SetRenderDrawColor(renderer, colors[2], colors[2], colors[2], 255);
				int x = (i % screen_width) * ss;
				int y = floor(i / screen_width) * ss;
				int wh = ss;
				SDL_Rect rect;
				rect.x = x;
				rect.y = y;
				rect.w = wh;
				rect.h = wh;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	window = SDL_CreateWindow("Chip-84 Windows Edition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * SCREEN_SCALE, 32 * SCREEN_SCALE, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		printf("Error creating window: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL) {
		SDL_DestroyWindow(window);
		printf("Error creating renderer: %s\n", SDL_GetError());
	}
	
	//loadProgram("rom/xochip/XOTEST");
	loadProgram("rom/INVADERS");
	//loadProgram("rom/schip8/TRAFFIC");
	
	SDL_Event e;
	int quit = 0;
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
					default:
						break;
				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			default: {}
			}
		}
		
		emulateCycle(100);
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		render_screen(renderer);
		SDL_RenderPresent(renderer);
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}