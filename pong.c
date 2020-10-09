#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <stdbool.h>

#include "vector.h"
#include "pong.h"

#define SCREEN_TITLE "Pong"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_RADIUS 20
#define PADDLE_WIDTH 80
#define PADDLE_VELOCITY 5
#define BALL_VELOCITY 10
#define MAX_SCORE 11

#define PADDLE_LOFFSET 10
#define PADDLE_THICKNESS 15

const SDL_Color white = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
const SDL_Color black = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
const Vector2 TOP_LEFT = {0, 0};
const Vector2 BOTTOM_RIGHT = {SCREEN_WIDTH, SCREEN_HEIGHT};
const Vector2 TOP_MID = {SCREEN_WIDTH/2, 0};
const Vector2 BOTTOM_MID = {SCREEN_WIDTH/2, SCREEN_HEIGHT};

Font *eight_bit_wonder;
Font *pong_score;

Renderer* new_renderer(SDL_Renderer *r, const SDL_Color *fg, const SDL_Color *bg) {
	Renderer *rend = malloc(sizeof(Renderer));
	rend->renderer = r;
	rend->fgcolor = fg;
	rend->bgcolor = bg;

	SDL_SetRenderDrawColor(r, bg->r, bg->g, bg->b, bg->a);
	SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(r, &screen);
	SDL_SetRenderDrawColor(r, fg->r, fg->g, fg->b, fg->a);
	SDL_RenderPresent(r);

	return rend;
}

void render_line(Renderer *r, const Vector2 *start, const Vector2 *end) {
	SDL_RenderDrawLine(r->renderer, (int)start->x, (int)start->y, (int)end->x, (int)end->y);
	SDL_RenderPresent(r->renderer);
}

void render_rect(Renderer *r, const Vector2 *tl, const Vector2 *br) {
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

void render_circle(Renderer *renderer, const Vector2 *c, const float r) {
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

void render_text(Renderer *r, const Vector2 *pos, Font *f, size_t size, const char* text) {
	
	TTF_Font *font = request_font(f, size);
	if (f == NULL) SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Font size requested does not exist");
	else {
		SDL_Surface *surf = TTF_RenderText_Blended(font, text, *(r->fgcolor));
		SDL_Rect textLoc = {(int)pos->x, (int)pos->y, surf->w, surf->h};
		SDL_Texture *texture = SDL_CreateTextureFromSurface(r->renderer, surf);
		SDL_RenderCopy(r->renderer, texture, NULL, &textLoc);
		SDL_RenderPresent(r->renderer);
		SDL_FreeSurface(surf);
	}
}

void fill_rect(Renderer *r, const Vector2 *tl, const Vector2 *br) {
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

void fill_circle(Renderer *renderer, const Vector2 *c, const float r) {
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

void render_clear_rect(Renderer *r, const Vector2 *tl, const Vector2 *br) {
	SDL_SetRenderDrawColor(r->renderer, r->bgcolor->r, r->bgcolor->g, r->bgcolor->b, r->bgcolor->a);
	fill_rect(r, tl, br);
	SDL_SetRenderDrawColor(r->renderer, r->fgcolor->r, r->fgcolor->g, r->fgcolor->b, r->fgcolor->a);
}

void render_clear_screen(Renderer *r) {
	render_clear_rect(r, &TOP_LEFT, &BOTTOM_RIGHT);
}

void destroy_renderer(Renderer *r) {
	SDL_DestroyRenderer(r->renderer);
	free(r);
}

void render_game(Renderer *r, Game *g) {
	render_clear_screen(r);
	render_rect(r, &TOP_LEFT, &BOTTOM_RIGHT);
	fill_rect(r, g->p1->tl, g->p1->br);
	fill_rect(r, g->p2->tl, g->p2->br);
	fill_circle(r, g->ball->centre, g->ball->radius);
	render_line(r, &TOP_MID, &BOTTOM_MID);

	if (g->state == START) {
		Vector2 t1Loc = {200, 100};
		Vector2 t2Loc = {170, 350};
		render_text(r, &t1Loc, eight_bit_wonder, 60, "PONG");
		render_text(r, &t2Loc, eight_bit_wonder, 15, "press any key to start");
	}
}

CircularBody* new_circular_body(Vector2 *pos, float radius) {
	CircularBody* body = malloc(sizeof(CircularBody));
	body->centre = pos;
	body->radius = radius;
	body->velocity = vec2_new(0, 0);
	return body;
}

RectangularBody* new_rectangular_body(Vector2 *tl, Vector2 *br) {
	RectangularBody* body = malloc(sizeof(RectangularBody));
	body->tl = tl;
	body->br = br;
	body->velocity = vec2_new(0, 0);
	return body;
}

void destroy_circular_body(CircularBody* body) {
	vec2_destroy(2, body->centre, body->velocity);
	free(body);
}

void destroy_rectangular_body(RectangularBody *body) {
	vec2_destroy(3, body->tl, body->br, body->velocity);
	free(body);
}

Game* new_game(int max, int pVel, int rad, int width) {
	Game *g = malloc(sizeof(Game));

	g->ball = new_circular_body(
		vec2_new(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), rad);
	g->p1 = new_rectangular_body( 
		vec2_new(PADDLE_LOFFSET, SCREEN_HEIGHT/2-width/2), 
		vec2_new(PADDLE_LOFFSET+PADDLE_THICKNESS, SCREEN_HEIGHT/2+width/2) );
	g->p2 = new_rectangular_body(
		vec2_new(SCREEN_WIDTH-PADDLE_LOFFSET-PADDLE_THICKNESS, SCREEN_HEIGHT/2-width/2),
		vec2_new(SCREEN_WIDTH-PADDLE_LOFFSET, SCREEN_HEIGHT/2+width/2) );
	
	g->p1score = g->p2score = 0;
	g->maxscore = max;
	g->pVel = pVel;
	g->state = START;
	
	return g;
}

void update_game(Game *game, SDL_Event *e) {
	if (e->type == SDL_KEYDOWN && game->state == START) {
		game->state = INGAME;
		srand(time(NULL));
		float ang = rand()*360/RAND_MAX;
		vec2_destroy(1, game->ball->velocity);
		game->ball->velocity = vec2_from_angle(BALL_VELOCITY, ang);
	}
	// ball update 
	vec2_add(game->ball->centre, game->ball->velocity);

	SDL_KeyboardEvent key = e->key;
	if (key.keysym.scancode == SDL_SCANCODE_UP) {
		game->p2->tl->y -= PADDLE_VELOCITY;
		game->p2->br->y -= PADDLE_VELOCITY; // Should refactor body to have width
	}
	// ball update
	// paddle update
	// 
}

void delete_game(Game *game) {
	destroy_circular_body(game->ball);
	destroy_rectangular_body(game->p1);
	destroy_rectangular_body(game->p2);
	free(game);
}

Font* new_font(char *path, size_t nsizes, ...) {
	Font *f = malloc(sizeof(Font));
	f->n = nsizes;
	f->sizes = malloc(sizeof(size_t)*nsizes);
	f->fonts = malloc(sizeof(TTF_Font*));

	va_list args;
	va_start(args, nsizes);
	for (size_t i=0; i<nsizes; i++) {
		int size = va_arg(args, size_t);
		f->fonts[i] = TTF_OpenFont(path, size);
		f->sizes[i] = size;
	}
	va_end(args);
	return f;
}

void destroy_font(Font *f) {
	for (int i=0; i<f->n; i++) {
		TTF_CloseFont(f->fonts[i]);
	}
	free(f->sizes);
	free(f->fonts);
	free(f);
}

TTF_Font* request_font(Font *f, size_t size) {
	// standard linear search here
	int index = -1;
	for (int i=0; i<f->n; i++) {
		if (f->sizes[i] == size) {
			index = i;
			break;
		}
	}
	if (index == -1) return NULL;
	else return f->fonts[index];
}
		
int init_graphics() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL: %s\n", 
						SDL_GetError());
		return -1;
	}
	
	if (TTF_Init() == -1) {
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not initalize SDL TTF: %s\n",
					    TTF_GetError());
		return -1;
	}
	return 0;
}

SDL_Window* create_window() {
	SDL_Window *w = SDL_CreateWindow(SCREEN_TITLE, 
									 SDL_WINDOWPOS_CENTERED, 
									 SDL_WINDOWPOS_CENTERED, 
									 SCREEN_WIDTH, 
									 SCREEN_HEIGHT, 
									 SDL_WINDOW_OPENGL);
	if (w == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", 
						SDL_GetError());
		return NULL;
	}
	return w;
}

void load_fonts() {
	eight_bit_wonder = new_font("res/8bitwonder.ttf", 2, 60, 15);
	pong_score = new_font("res/pong-score.ttf", 1, 40);
}

int main(int argc, char **argv) {

	SDL_Event e;
	bool quit = false;
	const int ticrate = 17;

	if (init_graphics() == -1) {
		return -1;
	}

	SDL_Window *w = create_window();
	if (w == NULL) return -1;
	
	load_fonts();
	SDL_Renderer* renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);

	Renderer *r = new_renderer(renderer, &white, &black);
	Game *g = new_game(MAX_SCORE, PADDLE_VELOCITY, BALL_RADIUS, PADDLE_WIDTH);
	
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}
		}
		update_game(g, &e);
		render_game(r, g);
		SDL_Delay(ticrate);
	}

	destroy_renderer(r);
	delete_game(g);
	SDL_DestroyWindow(w);
	SDL_Quit();

	return 0;
}
