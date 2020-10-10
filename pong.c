#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

#include "vector.h"
#include "pong.h"

#define SCREEN_TITLE "Pong"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_RADIUS 15
#define PADDLE_WIDTH 80
#define PADDLE_VELOCITY 15
#define BALL_VELOCITY 300
#define MAX_SCORE 11

#define PADDLE_LOFFSET 10
#define PADDLE_THICKNESS 15

const SDL_Color COLOR_WHITE = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
const SDL_Color COLOR_BLACK = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
const Vector2 TOP_LEFT = {0, 0};
const Vector2 BOTTOM_RIGHT = {SCREEN_WIDTH, SCREEN_HEIGHT};
const Vector2 TOP_MID = {SCREEN_WIDTH/2, 0};
const Vector2 BOTTOM_MID = {SCREEN_WIDTH/2, SCREEN_HEIGHT};

Font *eight_bit_wonder;
Font *pong_score;
Palette *palette;

Renderer* new_renderer(SDL_Renderer *r) {
	Renderer *rend = malloc(sizeof(Renderer));
	rend->renderer = r;
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
	renderer_set_color(rend, palette->colors[BLACK]);
	SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(r, &screen);
	renderer_set_color(rend, palette->colors[WHITE]);
	SDL_RenderPresent(r);
	return rend;
}

void render_line(Renderer *r, const Vector2 *start, const Vector2 *end) {
	SDL_RenderDrawLine(r->renderer, (int)start->x, (int)start->y, (int)end->x, (int)end->y);
}

void render_rect(Renderer *r, const Vector2 *tl, const Vector2 *br) {
	SDL_Rect rect = {(int)tl->x, (int)tl->y, (int)(br->x-tl->x), (int)(br->y-tl->y)};
	SDL_RenderDrawRect(r->renderer, &rect);
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
}

void render_text(Renderer *r, const Vector2 *pos, Font *f, size_t size, const char* text) {
	
	TTF_Font *font = request_font(f, size);
	if (f == NULL) SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Font size requested does not exist");
	else {
		uint8_t red, green, blue, alpha;
		SDL_GetRenderDrawColor(r->renderer, &red, &green, &blue, &alpha);
		SDL_Color color = {red, green, blue, alpha};
		SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
		SDL_Rect textLoc = {(int)pos->x, (int)pos->y, surf->w, surf->h};
		SDL_Texture *texture = SDL_CreateTextureFromSurface(r->renderer, surf);
		SDL_RenderCopy(r->renderer, texture, NULL, &textLoc);
		SDL_FreeSurface(surf);
	}
}

void fill_rect(Renderer *r, const Vector2 *tl, const Vector2 *br) {
	SDL_Rect rect = {(int)tl->x, (int)tl->y, (int)(br->x-tl->x), (int)(br->y-tl->y)};
	SDL_RenderFillRect(r->renderer, &rect);
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
}

void renderer_set_color(Renderer *r, SDL_Color *c) {
	SDL_SetRenderDrawColor(r->renderer, c->r, c->g, c->b, c->a);
}

void renderer_set_alpha(Renderer *rend, uint8_t a) {
	uint8_t r, g, b;
	SDL_GetRenderDrawColor(rend->renderer, &r, &g, &b, NULL);
	SDL_SetRenderDrawColor(rend->renderer, r, g, b, a);
}

void render_to_screen(Renderer *r) {
	SDL_RenderPresent(r->renderer);
}

void destroy_renderer(Renderer *r) {
	SDL_DestroyRenderer(r->renderer);
	free(r);
}

void render_game_objects(Renderer *r, Game *g) {
	render_rect(r, &TOP_LEFT, &BOTTOM_RIGHT);
	fill_rect(r, g->p1->tl, g->p1->br);
	fill_rect(r, g->p2->tl, g->p2->br);
	fill_circle(r, g->ball->centre, g->ball->radius);
	render_line(r, &TOP_MID, &BOTTOM_MID);
}

void render_scores(Renderer *r, Game *g) {
	Vector2 s1loc = {160, 20};
	Vector2 s2loc = {480, 20};
	char buf[3];
	snprintf(buf, 3, "%d", g->p1score);
	render_text(r, &s1loc, pong_score, 40, buf);
	snprintf(buf, 3, "%d", g->p2score);
	render_text(r, &s2loc, pong_score, 40, buf);
}
	
void render_game(Renderer *r, Game *g) {
	if (g->state == NONE) {
		renderer_set_alpha(r, 0x88);
		render_game_objects(r, g);

		renderer_set_alpha(r, 0xFF);
		Vector2 t1Loc = {200, 100};
		Vector2 t2Loc = {170, 350};
		render_text(r, &t1Loc, eight_bit_wonder, 60, "PONG");
		render_text(r, &t2Loc, eight_bit_wonder, 15, "press any key to start");
		render_to_screen(r);
		g->state = START;
	}
	else if (g->state == INGAME) {
		renderer_set_color(r, palette->colors[BLACK]);
		fill_rect(r, &TOP_LEFT, &BOTTOM_RIGHT);
		renderer_set_color(r, palette->colors[WHITE]);
		render_game_objects(r, g);
		render_scores(r, g);
		render_to_screen(r);
	}
	else if (g->state == END) {	
		renderer_set_color(r, palette->colors[BLACK]);
		fill_rect(r, &TOP_LEFT, &BOTTOM_RIGHT);
		renderer_set_color(r, palette->colors[WHITE]);
		renderer_set_alpha(r, 0x88);
		render_game_objects(r, g);
		render_scores(r, g);
		
		char winner[14] = "Player 2 wins";
		if (g->p1score > g->p2score) winner[7] = '1';
		Vector2 wLoc = {230, 300};
		Vector2 pLoc = {180, 330};
		renderer_set_alpha(r, 0xFF);
		render_text(r, &wLoc, eight_bit_wonder, 15, winner);
		render_text(r, &pLoc, eight_bit_wonder, 15, "Press P to play again");
		render_to_screen(r);
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
	g->state = NONE;
	g->lastUpdTime = -1;
	
	return g;
}

void reset_game(Game *game) {
	game->ball->centre->x = SCREEN_WIDTH/2;
	game->ball->centre->y = SCREEN_HEIGHT/2;
	srand(time(NULL));
	float ang = ((float)rand())/RAND_MAX * 120 - 60;
	int side = (int)(((float)rand())/RAND_MAX * 2);
	if (side == 0) ang += 180;
	vec2_destroy(1, game->ball->velocity);	
	game->ball->velocity = vec2_from_angle(BALL_VELOCITY, ang);
}
	
void update_game(Game *game, SDL_Event *e) {
	if (e->type == SDL_KEYDOWN) {
		if (game->state == START) {
			game->state = INGAME;
			reset_game(game);
		}
		else if(game->state == END && e->key.keysym.scancode == SDL_SCANCODE_P) {
			game->state = INGAME;
			game->p1score = game->p2score = 0;
			reset_game(game);
		}
	}

	const uint8_t *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_UP]) {
		game->p2->tl->y -= game->pVel;
		game->p2->br->y -= game->pVel; // Should refactor body to have width
	}
	if(state[SDL_SCANCODE_DOWN]) {
		game->p2->tl->y += game->pVel;
		game->p2->br->y += game->pVel;
	}
	if(state[SDL_SCANCODE_W]) {
		game->p1->tl->y -= game->pVel;
		game->p1->br->y -= game->pVel;
	}
	if(state[SDL_SCANCODE_S]) {
		game->p1->tl->y += game->pVel;
		game->p1->br->y += game->pVel;
	}
}

void update_game_physics(Game *game, int ticrate) {
	if (game->state != INGAME) return;

	Vector2 vel = { game->ball->velocity->x*ticrate/1000, 
				    game->ball->velocity->y*ticrate/1000 };
	vec2_add(game->ball->centre, &vel);

	float rad = game->ball->radius;
	int cx = iroundf(game->ball->centre->x);
	int cy = iroundf(game->ball->centre->y);
	int pwb2 = PADDLE_WIDTH/2;
	int p1yc = iroundf(game->p1->tl->y)+pwb2;
	int p2yc = iroundf(game->p2->tl->y)+pwb2;
	
	// collision detection
	if (cy >= SCREEN_HEIGHT-rad || cy <= rad) game->ball->velocity->y *= -1;
	if (cx >= SCREEN_WIDTH-PADDLE_LOFFSET-PADDLE_THICKNESS-rad && 
		(cy >= p2yc-pwb2 && cy <= p2yc+pwb2)) game->ball->velocity->x *= -1;
	if (cx <= PADDLE_LOFFSET+PADDLE_THICKNESS+rad && (cy >=p1yc-pwb2 && cy <= p1yc+pwb2)) 
		game->ball->velocity->x *= -1;
	
	// score detection
	if (cx <= PADDLE_LOFFSET+rad) {
		game->p2score++;
		reset_game(game);
	}
	else if(cx >= SCREEN_WIDTH-(PADDLE_LOFFSET+rad)) {
		game->p1score++;
		reset_game(game);
	}

	// win detection
	if (game->p1score >= game->maxscore || game->p2score >= game->maxscore) {
		game->state = END;
	}   
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

Palette* new_palette(size_t size, ...) {
	va_list args;
	va_start(args, size);
	Palette *p = malloc(sizeof(Palette));
	p->colors = malloc(sizeof(SDL_Color*) * size);
	p->n = size;
	for (size_t i=0; i<size; i++) {
		p->colors[i] = va_arg(args, SDL_Color*);
	}
	va_end(args);
	return p;
}

void destroy_palette(Palette *p) {
	free(p->colors);
	free(p);
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

void load_palette() {
	palette = new_palette(2, &COLOR_WHITE, &COLOR_BLACK);
}

int main(int argc, char **argv) {

	SDL_Event e;
	bool quit = false;
	const int ticrate = 10;

	if (init_graphics() == -1) {
		return -1;
	}

	SDL_Window *w = create_window();
	if (w == NULL) return -1;
	
	load_fonts();
	load_palette();
	SDL_Renderer* renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);

	Renderer *r = new_renderer(renderer);
	Game *g = new_game(MAX_SCORE, PADDLE_VELOCITY, BALL_RADIUS, PADDLE_WIDTH);
	
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else {
				update_game(g, &e);
			}
		}
		update_game_physics(g, ticrate);
		render_game(r, g);
		SDL_Delay(ticrate);
	}

	destroy_renderer(r);
	destroy_font(eight_bit_wonder);
	destroy_font(pong_score);
	destroy_palette(palette);
	delete_game(g);
	SDL_DestroyWindow(w);
	SDL_Quit();

	return 0;
}
