#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "vector.h"
#include "pong.h"

#define SCREEN_TITLE "Pong"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

Renderer* new_renderer(SDL_Renderer *r, TTF_Font *f, 
                       SDL_Color *fg, SDL_Color *bg) {
	Renderer *rend = malloc(sizeof(Renderer));
	rend->renderer = r;
	rend->font = f;
	rend->fgcolor = fg;
	rend->bgcolor = bg;

	SDL_SetRenderDrawColor(r, bg->r, bg->g, bg->b, bg->a);
	SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(r, &screen);
	SDL_SetRenderDrawColor(r, fg->r, fg->g, fg->b, fg->a);
	SDL_RenderPresent(r);

	return rend;
}

void render_line(Renderer *r, Vector2 *start, Vector2 *end) {
	SDL_RenderDrawLine(r->renderer, (int)start->x, (int)start->y, (int)end->x, (int)end->y);
	SDL_RenderPresent(r->renderer);
}

void render_rect(Renderer *r, Vector2 *tl, Vector2 *br) {
	SDL_Rect rect = {(int)tl->x, (int)tl->y, (int)(br->x-tl->x), (int)(br->y-tl->y)};
	SDL_RenderDrawRect(r->renderer, &rect);
	SDL_RenderPresent(r->renderer);
}

void sym_render_pts(SDL_Renderer *renderer, int x, int y, int dx, int dy) {
	SDL_RenderDrawPoint(renderer, x+dx, y+dy);
	SDL_RenderDrawPoint(renderer, x-dx, y+dy);
	SDL_RenderDrawPoint(renderer, x+dx, y-dy);
	SDL_RenderDrawPoint(renderer, x-dx, y-dy);
	SDL_RenderDrawPoint(renderer, x+dy, y+dx);
	SDL_RenderDrawPoint(renderer, x-dy, y+dx);
	SDL_RenderDrawPoint(renderer, x+dy, y-dx);
	SDL_RenderDrawPoint(renderer, x-dy, y-dx);
}

int iroundf(float f) {
	return (int)(f+0.5);
}

void render_circle(Renderer *renderer, Vector2 *c, float r) {
	int x0 = iroundf(c->x);
	int y0 = iroundf(c->y);

	float x=0, y=r;
	sym_render_pts(renderer->renderer, x0, y0, x, iroundf(y));
	while (x <= y) {
		x++;
		y = sqrt(r*r-x*x);
		sym_render_pts(renderer->renderer, x0, y0, x, iroundf(y));
	}
	SDL_RenderPresent(renderer->renderer);
}

void render_text(Renderer *r, Vector2 *pos, char* text) {
	SDL_Surface *surf = TTF_RenderText_Blended(r->font, text, *(r->fgcolor));
	SDL_Rect textLoc = {(int)pos->x, (int)pos->y, surf->w, surf->h};
	SDL_Texture *texture = SDL_CreateTextureFromSurface(r->renderer, surf);
	SDL_RenderCopy(r->renderer, texture, NULL, &textLoc);
	SDL_RenderPresent(r->renderer);
	SDL_FreeSurface(surf);
}

void fill_rect(Renderer *r, Vector2 *tl, Vector2 *br) {
	SDL_Rect rect = {(int)tl->x, (int)tl->y, (int)(br->x-tl->x), (int)(br->y-tl->y)};
	SDL_RenderFillRect(r->renderer, &rect);
	SDL_RenderPresent(r->renderer);
}

void sym_fill_pts(SDL_Renderer *r, int x0, int y0, int x, int y) {
	SDL_Rect rect1 = {x0-x, y0-y, 2*x, 2*y};
	SDL_Rect rect2 = {x0-y, y0-x, 2*y, 2*x};
	SDL_RenderFillRect(r, &rect1);
	SDL_RenderFillRect(r, &rect2);
}

void fill_circle(Renderer *renderer, Vector2 *c, float r) {
	int x0 = iroundf(c->x);
	int y0 = iroundf(c->y);

	float x=0, y=r;
	sym_fill_pts(renderer->renderer, x0, y0, x, iroundf(y));
	while (x <= y) {
		x++;
		y = sqrt(r*r-x*x);
		sym_fill_pts(renderer->renderer, x0, y0, x, iroundf(y));
	}
	SDL_RenderPresent(renderer->renderer);
}

void destroy_renderer(Renderer *r) {
	SDL_DestroyRenderer(r->renderer);
	TTF_CloseFont(r->font);
	free(r);
}

int main(int argc, char **argv) {
	// test
	SDL_Event e;
	bool quit = false;
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Window *w = SDL_CreateWindow(SCREEN_TITLE, 
									 SDL_WINDOWPOS_CENTERED, 
									 SDL_WINDOWPOS_CENTERED, 
									 SCREEN_WIDTH, 
									 SCREEN_HEIGHT, 
									 SDL_WINDOW_OPENGL);
	if (w == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	TTF_Font *font = TTF_OpenFont("res/8bitwonder.ttf", 30);
	if (font == NULL) printf("Font loaded is null: %s\n", TTF_GetError());
	static SDL_Color white = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
	static SDL_Color black = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
	Renderer *r = new_renderer(renderer, font, &white, &black);
	
	render_line(r, vec2_new(100,100), vec2_new(200, 200));
	render_text(r, vec2_new(250,150), "Hello World");
	render_text(r, vec2_new(0,200), "Munni doesn't like this");
	render_rect(r, vec2_new(100,300), vec2_new(250, 450));
	fill_rect(r, vec2_new(250, 190), vec2_new(575, 195));
	render_rect(r, vec2_new(0,0), vec2_new(640, 480));
	fill_circle(r, vec2_new(175,375), 50);

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}
		}
	}

	destroy_renderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();

	return 0;
}
