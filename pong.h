#ifndef PONG_H
#define PONG_H

#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
	SDL_Color *fgcolor;
	SDL_Color *bgcolor;
	TTF_Font *font;
	SDL_Renderer *renderer;
} Renderer;

Renderer* new_renderer(SDL_Renderer *r, TTF_Font *f, SDL_Color *fg, SDL_Color *bg);

void render_line   (Renderer *r, Vector2 *start, Vector2 *end);
void render_rect   (Renderer *r, Vector2 *top_left, Vector2 *bottom_right);
void render_circle (Renderer *r, Vector2 *centre, float radius);
void render_text   (Renderer *r, Vector2 *pos, char* text);
void fill_rect     (Renderer *r, Vector2 *top_left, Vector2 *bottom_right);
void fill_circle   (Renderer *r, Vector2 *centre, float radius);

void destroy_renderer (Renderer *r);
#endif
