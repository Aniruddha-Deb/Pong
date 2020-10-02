#include "panel.hpp"
#include <cstddef>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stack>

#define MM_BG_PATH "res/main_menu.png"
#define MM_FONT_PATH "res/8bitwonder.ttf"

Panel::Panel(std::stack<Panel*> *stack, SDL_Renderer *rend) {
	pstack = stack;
	renderer = rend;
}

GamePanel::GamePanel(std::stack<Panel*> *stack, SDL_Renderer *rend): 
	Panel(stack, rend) {
	// TODO init GamePanel stuffs
}

MainMenuPanel::MainMenuPanel(std::stack<Panel*> *stack, SDL_Renderer *rend):
	Panel(stack, rend) {
	background = IMG_LoadTexture(renderer, MM_BG_PATH);

	font = TTF_OpenFont(MM_FONT_PATH, 30);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	menuitems = new MenuItem*[3];
	menuitems[0] = new MenuItem("1 Player", {200, 250}, NULL);

	menuitems[0]->render(font, renderer);
}

MenuItem::MenuItem(std::string t, Vec2 p, std::string *opt) {
	text = t;
	pos = p;
	options = opt;
	color = {255, 255, 255};
}

MenuItem::~MenuItem() {
}

void MenuItem::render(TTF_Font* font, SDL_Renderer *r) {
	SDL_Surface *text = TTF_RenderText_Blended(font, MenuItem::text.c_str(), color);
	SDL_Rect textLoc = {(int)pos.x, (int)pos.y, text->w, text->h};
	SDL_Texture *texture = SDL_CreateTextureFromSurface(r, text);
	SDL_FreeSurface(text);
	SDL_RenderCopy(r, texture, NULL, &textLoc);
	SDL_RenderPresent(r);
	std::cout << "rendered menuitem" << MenuItem::text << std::endl;
}

void MenuItem::renderActive(TTF_Font* font, SDL_Renderer *r) {
	render(font, r);
	// TODO render dot
}

void GamePanel::update(SDL_Event *e) {
	std::cout << "GamePanel updated" << std::endl;
}

void MainMenuPanel::update(SDL_Event *e) {
	std::cout << "MainMenuPanel updated" << std::endl;
}

void GamePanel::destroy() {
	std::cout << "GamePanel destroyed" << std::endl;
}

void MainMenuPanel::destroy() {
	std::cout << "MainMenuPanel destroyed" << std::endl;
}
