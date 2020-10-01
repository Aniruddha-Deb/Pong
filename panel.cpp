#include "panel.hpp"

Panel::Panel(PanelStack *stack, SDL_Renderer *rend) {
	pstack = stack;
	renderer = rend;
}

void updatePanel(SDL_Event *e) {
	
