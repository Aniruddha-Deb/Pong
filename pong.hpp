#ifndef PONG_HPP
#define PONG_HPP

#include <stack>

#define WIDTH 640
#define HEIGHT 480
#define TITLE "Pong"

class Game {
	private:
	SDL_Window *win;
	std::stack<Panel*> *pstack;
	
	void initWindow();
	void createPanelStack(SDL_Renderer *renderer);
	void launchMainLoop();

	public:
	Game();
	~Game();
	void launch();
};

#endif
