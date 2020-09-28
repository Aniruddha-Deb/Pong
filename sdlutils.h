#include <SDL2/SDL.h>
#include <stddef.h>

SDL_Window* create_window(char* name, int width, int height) {
	SDL_Window *w = SDL_CreateWindow(name, 
									 SDL_WINDOWPOS_CENTERED, 
									 SDL_WINDOWPOS_CENTERED, 
									 width, 
									 height, 
									 SDL_WINDOW_OPENGL);
	if (w == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return NULL;
	}
	return w;
}
