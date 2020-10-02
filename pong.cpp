#include <iostream>
#include <stack>
#include <SDL2/SDL.h>
#include "panel.hpp"
#include "pong.hpp"

Game::Game() {
	// TODO
}

Game::~Game() {
	while (!pstack->empty()) {
		pstack->top()->destroy();
		pstack->pop();
	}
	delete pstack;
	SDL_DestroyWindow(win);
	SDL_Quit();
}

void Game::launch() {
	SDL_Init(SDL_INIT_VIDEO);
	initWindow();
	SDL_Renderer *renderer = SDL_GetRenderer(win);
	createPanelStack(renderer);
	launchMainLoop();
}

void Game::initWindow() {
	win = SDL_CreateWindow(
		TITLE, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_OPENGL
	);
}

void Game::createPanelStack(SDL_Renderer *renderer) {
	pstack = new std::stack<Panel*>;
	MainMenuPanel *mmp = new MainMenuPanel(pstack, renderer);
	pstack->push(mmp);
}

void Game::launchMainLoop() {
	SDL_Event e;
	bool playing = true;
	while (playing) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				// exit pong
				delete this;
				playing = false;
				break;
			}
			else {
				pstack->top()->update(&e);
			}
		}
		SDL_Delay(30);
	}
}

int main() {
	Game *g = new Game();
	g->launch();
	return 0;
}
