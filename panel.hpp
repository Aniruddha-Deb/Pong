#ifndef PANEL_HPP
#define PANEL_HPP

#include <stack>
#include "physics.hpp"
#include <SDL2/SDL.h>

class Panel {
	protected:
	std::stack<Panel*> *pstack;
	SDL_Renderer *renderer;

	public:
	Panel(std::stack<Panel*>*, SDL_Renderer*);
	virtual void destroy() = 0;
	virtual void update(SDL_Event*) = 0;
};

class GamePanel: public Panel {
	Body *ball;
	Body *paddle1;
	Body *paddle2;

	public:
	using Panel::Panel;
	void update(SDL_Event*) override;
	void destroy() override;
};

class MainMenuPanel: public Panel {
	public:
	using Panel::Panel;
	void update(SDL_Event*) override;
	void destroy() override;
};
#endif
