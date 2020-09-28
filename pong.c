#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "vector.h"
#include "sdlutils.h"
#include <SDL2/SDL_image.h>

typedef struct {
	SDL_Texture *background;
} Scene;

typedef struct {
	Scene **scenes;
	int size;
	int head;
} SceneStack;

SceneStack* create_scene_stack(int max_size) {
	SceneStack *scene_stack = malloc(sizeof(SceneStack));
	scene_stack->scenes = malloc(sizeof(Scene*) * max_size);
	scene_stack->size = max_size;
	scene_stack->head = 0;
	return scene_stack;
}

Scene* pop_scene(SceneStack* scene_stack) {
	if (head <= 0) return NULL;
	return scene_stack->scenes[scene_stack->head--];
}

int push_scene(SceneStack* scene_stack, Scene *scene) {
	if (head >= size-1) return -1; // fail
	scene_stack->scenes[head++] = scene;
	return 0;
}

void render_main_menu(SDL_Window *window) {
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture *img = IMG_LoadTexture(renderer, "main_menu.png");
	SDL_Rect imgLoc = {0, 0, 640, 480};
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, img, NULL, &imgLoc);
	SDL_RenderPresent(renderer);
}

int show_main_menu(SDL_Window *window) {
	render_main_menu(window);
	printf("Rendered\n");
	while(1) {}
	return 0;
}

SceneStack* init_scene_stack() {
	// TODO
}

void enter_game_loop(SDL_Window *window) {
	Scene *main_menu_scene = create_main_menu_scene();
	Scene *game_scene = create_game_scene();
	SceneStack *scene_stack = create_scene_stack(3);
	push_scene(scene_stack, game_scene);
	push_scene(scene_stack, main_menu_scene);
	SDL_Event e;
	while (1) {
		render_scene_stack(scene_stack);
		while (SDL_PollEvent(&e)) {
			game_scene = 
			// This needs OOP. Serious OOP.....
			// Use C++ here lol...
		}
		update_scene_stack(scene_stack
	

int main(int argc, char** argv) {
	SDL_Window *window = create_window("Pong", 640, 480);
	show_main_menu(window);
	return 0;
}
