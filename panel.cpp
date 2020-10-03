#include "panel.hpp"
#include <cstddef>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stack>

#define MM_BG_PATH "res/main_menu.png"
#define MM_FONT_PATH "res/8bitwonder.ttf"
#define FOCUS "res/focus.png"

void initMenuItems(std::vector<MenuItem*>*);

const std::string mainMenuItems[] = { "1 Player", "2 Player", "Settings", "Test" };
const Vec2 menuItemPositions[] = { {200,250}, {200,300}, {200,350}, {200, 400} };
const SDL_Color fgColor = {255, 255, 255}; 

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
	
	menuitems = new std::vector<MenuItem*>;
	initMenuItems(menuitems);
	activeMenuItem = 0;
	prevActiveMenuItem = -1;

	renderMenuItems();
}

void MainMenuPanel::renderMenuItems() {
	for (size_t i=0; i<menuitems->size(); i++) {
		if (i == activeMenuItem) {
			menuitems->at(i)->renderActive(font, renderer);
		}
		else if(i == prevActiveMenuItem) {
			std::cout << "Prev active MI at " << i << std::endl;
			menuitems->at(i)->renderInactive(font, renderer);
		}
		else {
			menuitems->at(i)->render(font, renderer);
		}
	}
}

void initMenuItems(std::vector<MenuItem*> *menuitems) {
	MenuItem *m1 = new MenuItem(mainMenuItems[0], menuItemPositions[0]);
	MenuItem *m2 = new MenuItem(mainMenuItems[1], menuItemPositions[1]);
	MenuItem *m3 = new MenuItem(mainMenuItems[2], menuItemPositions[2]);
	MenuItem *m4 = new MenuItem(mainMenuItems[3], menuItemPositions[3]);
	menuitems->push_back(m1);
	menuitems->push_back(m2);
	menuitems->push_back(m3);
	menuitems->push_back(m4);
}

void MainMenuPanel::addMenuItem(MenuItem *mItem) {
	menuitems->push_back(mItem);
}

MenuItem::MenuItem(std::string t, Vec2 p) {
	text = t;
	pos = p;
	color = fgColor;
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
}

void MenuItem::renderActive(TTF_Font* font, SDL_Renderer *r) {
	render(font, r);
	SDL_Texture *dot = IMG_LoadTexture(r, FOCUS);
	int w,h;
	SDL_QueryTexture(dot, NULL, NULL, &w, &h);
	SDL_Rect texr = {(int)pos.x-w-6, (int)pos.y+6, w, h};
	SDL_RenderCopy(r, dot, NULL, &texr);
	SDL_RenderPresent(r);
}

void MenuItem::renderInactive(TTF_Font* font, SDL_Renderer *r) {
	render(font, r);
	SDL_Texture *dot = IMG_LoadTexture(r, FOCUS);
	int w,h;
	SDL_QueryTexture(dot, NULL, NULL, &w, &h);
	SDL_Rect texr = {(int)pos.x-w-6, (int)pos.y+6, w, h};
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderFillRect(r, &texr);
	std::cout << "Filled rect at " << texr.x << ", " << texr.y << std::endl;
	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
}

void GamePanel::update(SDL_Event *e) {
	std::cout << "GamePanel updated" << std::endl;
}

void MainMenuPanel::update(SDL_Event *e) {
	if (e->type == SDL_KEYDOWN) {
		prevActiveMenuItem = activeMenuItem;
		if (e->key.keysym.sym == SDLK_DOWN) {
			activeMenuItem = (activeMenuItem+1)%menuitems->size();
			renderMenuItems();
		}
		else if(e->key.keysym.sym == SDLK_UP) {
			activeMenuItem = (activeMenuItem-1);
			if (activeMenuItem < 0) {
				std::cout << "Went neg" << std::endl;
				activeMenuItem = (activeMenuItem+menuitems->size());
			}
			activeMenuItem %= menuitems->size();
			renderMenuItems();
		}
	}
}

void GamePanel::destroy() {
	std::cout << "GamePanel destroyed" << std::endl;
}

void MainMenuPanel::destroy() {
	std::cout << "MainMenuPanel destroyed" << std::endl;
}
