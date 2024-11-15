#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "player.h"
#include "constants.h"
#include "texture.h"
#include "ray.h"

// map
void printMap(int (*map)[8]); 
void initMap(int (*map)[8]);
void drawMap(int (*map)[8], SDL_Rect rect, SDL_Renderer *renderer, int w, int h, int offset_x, int offset_y);

// player
void initPlayer(struct player *Player);
void drawPlayer(SDL_Renderer *renderer, struct player *Player, SDL_Rect player);
void updatePlayerPosition(struct player *Player, SDL_Rect *player, int (*map)[8]);

// raycasting
struct ray castRay(SDL_Renderer *renderer, struct player *Player, SDL_Rect *player, int (*map)[8], float ray_angle, struct ray *Ray);
void drawRay(SDL_Renderer *renderer, struct ray *Ray, float ray_angle);
void castRays(SDL_Renderer *renderer, struct player *Player, SDL_Rect *player, int(*map)[8], struct ray *Ray, _Bool draw); 

// handle utils (window and renderer)
_Bool windowError(SDL_Window *window); 
_Bool rendererError(SDL_Renderer *renderer, SDL_Window *window); 

#endif