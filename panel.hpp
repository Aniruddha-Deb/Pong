#ifndef PANEL_HPP
#define PANEL_HPP

#include <SDL2/SDL.h>
#include "physics.hpp"

class PanelStack;

class Panel {
	PanelStack *pstack;
	SDL_Renderer *renderer;

	public:
	Panel(PanelStack*, SDL_Renderer*);
	virtual void updatePanel(SDL_Event*) = 0;
};

class PanelStack {
	public:
	Panel **panels;
	int size;
	int head;

	PanelStack(int size);
	~PanelStack();
	Panel* pop();
	int push(Panel* panel);
	int resize(int newSize);
};

class GamePanel: public Panel {
	Body *ball;
	Body *paddle1;
	Body *paddle2;

	public:
	GamePanel(PanelStack*, SDL_Renderer*);
};

class MainMenuPanel: public Panel {
	public:
	MainMenuPanel(PanelStack*, SDL_Renderer*);
};	

class SettingsMenuPanel: public Panel {
	public:
	SettingsMenuPanel(PanelStack*, SDL_Renderer*);
};

#endif
