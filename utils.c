#include <stdio.h>
#include "utils.h"
#include "player.h"
#include "ray.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

void printMap(int (*map)[8]){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            printf("%d", map[i][j]); 
        }
        printf("\n"); 
    }
}

void initMap(int (*map)[8]){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (i == 0 || i == 7 || j == 0 || j == 7){
                map[i][j] = 1; 
            } else {
                map[i][j] = 0; 
            }
        }
    }
}

void drawMap(int (*map)[8], SDL_Rect rect, SDL_Renderer *renderer, int w, int h, int offset_x, int offset_y){
    rect.w = w; 
    rect.h = h; 
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            rect.x = i*rect.w + offset_x;
            rect.y = j*rect.h + offset_y;
            switch(map[j][i]){
                case 1:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 90, 255);  
                    SDL_RenderFillRect(renderer,&rect);   
                    break; 
                case 2:
                    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);  
                    SDL_RenderFillRect(renderer,&rect);   
                    break; 
                case 3: 
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  
                    SDL_RenderFillRect(renderer,&rect);   
                    break; 
                case 4: 
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  
                    SDL_RenderFillRect(renderer,&rect);   
            }
            // grid
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); 
            SDL_RenderDrawRect(renderer, &rect); 
        }
    }
}

void updatePlayerPosition(struct player *Player, SDL_Rect *player, int(*map)[8]) {
    int new_x = Player->pos_x; 
    int new_y = Player->pos_y; 

    if (Player->movingForward) {
        Player->speed = MOVEMENT_SPEED;
        new_x = Player->pos_x + Player->speed * cos(Player->angle);
        new_y = Player->pos_y + Player->speed * sin(Player->angle);
    }
    if (Player->movingBackward) {
        Player->speed = -MOVEMENT_SPEED;
        new_x = Player->pos_x + Player->speed * cos(Player->angle);
        new_y = Player->pos_y + Player->speed * sin(Player->angle);
    }

    // Check collision before updating position
    int map_X = (new_x / (SIZE_H / 8));
    int map_Y = (new_y / (SIZE_H / 8)); 

    if (map_X >= 0 && map_X < 8 && map_Y >= 0 && map_Y < 8) {
        if (map[map_Y][map_X] == 0){
            Player->pos_x = new_x; 
            Player->pos_y = new_y; 
            player->x = Player->pos_x; 
            player->y = Player->pos_y; 
        }
    }


    if (Player->movingLeft) {
        Player->angle -= ROTATION_SPEED;
        player->x = Player->pos_x;
        player->y = Player->pos_y;
    }
    if (Player->movingRight) {
        Player->angle += ROTATION_SPEED;
        player->x = Player->pos_x;
        player->y = Player->pos_y;
    }
}


void initPlayer(struct player *Player){
    Player->pos_x = (SIZE_W / 2); 
    Player->pos_y = (SIZE_H / 2); 
    Player->angle = 0.0f; 
    Player->speed = 0.0f; 
}

void drawPlayer(SDL_Renderer *renderer, struct player *Player, SDL_Rect player){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 

    SDL_Rect miniPlayer; 
    miniPlayer.x = (player.x / 5)+(SIZE_W - (SIZE_W / 5)); // 
    miniPlayer.y = (player.y / 5)+0;  // 0px is offset_y of minimap
    miniPlayer.w = player.w / 5; 
    miniPlayer.h = player.h / 5; 

    int x1 = miniPlayer.x + (miniPlayer.w / 2); 
    int y1 = miniPlayer.y + (miniPlayer.h / 2);
    int x2 = x1 + 10 * cos(Player->angle); 
    int y2 = y1 + 10 * sin(Player->angle);  

    SDL_RenderFillRect(renderer, &miniPlayer); 
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2); 
    
}

struct ray castRay(SDL_Renderer *renderer, struct player *Player, SDL_Rect *player, int (*map)[8], float ray_angle, struct ray *Ray){
    Ray->ray_x1 = Player->pos_x + (player->w / 2); 
    Ray->ray_y1 = Player->pos_y + (player->h / 2); 

    Ray->ray_x2 = Ray->ray_x1; 
    Ray->ray_y2 = Ray->ray_y1; 

    float ray_step = 1.0f; 
    float max_dist = 1000.0f; 

    while(ray_step < max_dist){
        Ray->ray_x2 = Ray->ray_x1 + ray_step * cos(ray_angle); 
        Ray->ray_y2 = Ray->ray_y1 + ray_step * sin(ray_angle); 

        int map_X = ((int)Ray->ray_x2) / (SIZE_H / 8); // 60 pixel is a cell
        int map_Y = ((int)Ray->ray_y2) / (SIZE_H / 8); 

        // Check boundaries
        if (map_X < 0 || map_X >= 8 || map_Y < 0 || map_Y >= 8){
            break; 
        }

        if (map[map_Y][map_X] != 0) {
            break; 
        }
        
        ray_step += 1.0f; // increment by 1 pixel if don't find wall
    }
    return *Ray; 
}


void drawRay(SDL_Renderer *renderer, struct ray *Ray, float ray_angle) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    // Scale both position and length by 1/5
    float ray_length = sqrt(pow(Ray->ray_x2 - Ray->ray_x1, 2) + pow(Ray->ray_y2 - Ray->ray_y1, 2)) / 5;
    int x1 = (Ray->ray_x1 / 5) + (SIZE_W - (SIZE_W / 5));
    int y1 = (Ray->ray_y1 / 5) + 0;
    int x2 = x1 + ray_length * cos(ray_angle);
    int y2 = y1 + ray_length * sin(ray_angle);

    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void castRays(SDL_Renderer *renderer, struct player *Player, SDL_Rect *player, int (*map)[8], struct ray *Ray, _Bool draw) {
    float FOV = M_PI/3; // 60 degrees FOV
    int num_rays = SIZE_W - (SIZE_W / 5); // Number of rays to cast
    float ray_angle = Player->angle - FOV/2;
    float angle_step = FOV/num_rays;
    int x1_3d, x2_3d, y1_3d, y2_3d; 
    for(int i = 0; i < num_rays; i++) {
        castRay(renderer, Player, player, map, ray_angle, Ray);
        if (draw){
            drawRay(renderer, Ray, ray_angle); 
        }

        float distance = sqrtf((Ray->ray_x2 - Ray->ray_x1)*(Ray->ray_x2 - Ray->ray_x1) + (Ray->ray_y2 - Ray->ray_y1)*(Ray->ray_y2 - Ray->ray_y1)); 
        float corrected_distance = distance * cos(ray_angle - Player->angle);
        int line_height = (int)((SIZE_H * 12.5f)/ corrected_distance);
        int line_start = (SIZE_H / 2) - (line_height / 2); 
        int line_end = line_start + line_height; 

        int map_X = ((int)Ray->ray_x2) / (SIZE_H / 8); // 60 pixel is a cell
        int map_Y = ((int)Ray->ray_y2) / (SIZE_H / 8);

        if (map[map_Y][map_X] == 1) {
            SDL_SetRenderDrawColor(renderer, 255 - (int)(corrected_distance * 0.6), 0, 90, 255); 
        } 
        if (map[map_Y][map_X] == 2) {
            SDL_SetRenderDrawColor(renderer, 155 - (int)(corrected_distance * 0.6), 155-(int)(corrected_distance * 0.6), 155-(int)(corrected_distance * 0.6), 255); 
        } 
        if (map[map_Y][map_X] == 3) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255 - (int)(corrected_distance * 0.6), 255); 
        }
        if (map[map_Y][map_X] == 4) {
            SDL_SetRenderDrawColor(renderer, 0, 255 - (int)(corrected_distance * 0.6), 0, 255); 
        } 
                

        x1_3d = i * ((SIZE_W - (SIZE_W / 5)) / num_rays);
        x2_3d = x1_3d; 
        y1_3d = line_start; 
        y2_3d = line_end; 
        SDL_RenderDrawLine(renderer, x1_3d, y1_3d, x2_3d, y2_3d); 

        ray_angle += angle_step;
    }
}




_Bool windowError(SDL_Window *window){
    if (window == NULL ) {
        printf("Error creating window"); 
        printf("%s\n",SDL_GetError()); 
        SDL_Quit();
        return 1; 
    }
    return 0; 
}

_Bool rendererError(SDL_Renderer *renderer, SDL_Window *window){
    if (renderer == NULL){
        printf("Error creating renderer"); 
        printf("%s\n",SDL_GetError());  
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1; 
    }
    return 0; 
}

