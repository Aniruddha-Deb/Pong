#include "panel.hpp"
#include <cstddef>
#include <iostream>
#include <stack>

Panel::Panel(std::stack<Panel*> *stack, SDL_Renderer *rend) {
	pstack = stack;
	renderer = rend;
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
