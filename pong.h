#ifndef PONG_H
#define PONG_H

#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>

typedef enum {
	START, INGAME, END
} GameState;

typedef struct {
	TTF_Font **fonts;
	size_t *sizes;
	size_t n;
} Font;

typedef struct {
	const SDL_Color *fgcolor;
	const SDL_Color *bgcolor;
	SDL_Renderer *renderer;
} Renderer;

typedef struct {
	Vector2 *centre;
	float radius;
	Vector2 *velocity;
} CircularBody;

typedef struct {
	Vector2 *tl;
	Vector2 *br;
	Vector2 *velocity;
} RectangularBody;

typedef struct {
	CircularBody *ball;
	RectangularBody *p1;
	RectangularBody *p2;
	uint16_t p1score, p2score;
	uint16_t maxscore;
	long lastUpdTime;
	int pVel;
	GameState state;
} Game;

Font* new_font(char *path, size_t nsizes, ...);
void destroy_font(Font *f);
TTF_Font* request_font(Font *f, size_t size);

Renderer* new_renderer(SDL_Renderer *r, const SDL_Color *fg, const SDL_Color *bg);

void render_clear_rect(Renderer *r, const Vector2 *start, const Vector2 *end);

void render_line   (Renderer *r, const Vector2 *start, const Vector2 *end);
void render_rect   (Renderer *r, const Vector2 *top_left, const Vector2 *bottom_right);
void render_circle (Renderer *r, const Vector2 *centre, const float radius);
void render_text   (Renderer *r, const Vector2 *pos, Font *font, size_t size, const char* text);
void fill_rect     (Renderer *r, const Vector2 *top_left, const Vector2 *bottom_right);
void fill_circle   (Renderer *r, const Vector2 *centre, const float radius);

void destroy_renderer (Renderer *r);

CircularBody* new_circular_body(Vector2 *pos, float radius);
RectangularBody* new_rectangular_body(Vector2 *tl, Vector2 *br);

void destroy_circular_body(CircularBody* body);
void destroy_rectangular_body(RectangularBody* body);

Game* new_game(int max, int pVel, int rad, int width);
void  update_game(Game *game, SDL_Event *e);
void  delete_game(Game *game);

#endif
