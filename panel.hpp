#ifndef PANEL_HPP
#define PANEL_HPP

#include <stack>
#include "physics.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

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
	GamePanel(std::stack<Panel*>*, SDL_Renderer*);
	void update(SDL_Event*) override;
	void destroy() override;
};

class MenuItem {
	public:
	std::string text;
	SDL_Color color;
	Vec2 pos;
	std::vector<std::string> options;
	int currOpn;

	MenuItem(std::string t, Vec2 pos);
	~MenuItem();
	void render(TTF_Font*, SDL_Renderer*);
	void renderActive(TTF_Font*, SDL_Renderer*);
	void renderInactive(TTF_Font*, SDL_Renderer*);
	std::string getActiveOption();
};

class MainMenuPanel: public Panel {
	private:
	SDL_Texture *background;
	std::vector<MenuItem*> *menuitems;
	int activeMenuItem;
	int prevActiveMenuItem;
	TTF_Font *font;

	public:
	MainMenuPanel(std::stack<Panel*>*, SDL_Renderer*);
	void addMenuItem(MenuItem*);
	void renderMenuItems();
	void update(SDL_Event*) override;
	void destroy() override;
};
#endif
