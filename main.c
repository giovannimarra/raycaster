#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "utils.h"
#include "constants.h"
#include "player.h"
#include "ray.h"



// window constants
const int SIZE_W = 1024;
const int SIZE_H = 720;

// matrix
int map[8][8] = { 
    {1, 1, 1, 4, 4, 1, 1, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 0, 2, 0, 2, 0, 1}, 
    {4, 0, 0, 0, 0, 0, 0, 4}, 
    {4, 0, 3, 3, 0, 2, 0, 4}, 
    {1, 0, 3, 3, 0, 0, 0, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 1, 1, 4, 4, 1, 1, 1} };


SDL_Rect minimap; 
SDL_Rect map2d; 
SDL_Rect player;


// definisci player 
struct player Player; 

struct ray Ray;

//struct WallTexture wallTextures[4]; 

int main(){
    // Init video 
    if (SDL_Init((SDL_INIT_VIDEO < 0))){
        printf("Error initializing SDL"); 
        printf("%s\n",SDL_GetError()); 
    }


    // window
    SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE_W, SIZE_H, SDL_WINDOW_SHOWN);
    if (windowError(window)){
        return 1; 
    }

    // renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED); 
    if (rendererError(renderer, window)){
        return 1; 
    }
/*
    IMG_Init(IMG_INIT_PNG);
    wallTextures[0].texture = IMG_LoadTexture(renderer, "wall1.png");
    wallTextures[1].texture = IMG_LoadTexture(renderer, "wall2.png");
    wallTextures[2].texture = IMG_LoadTexture(renderer, "wall3.png");
    wallTextures[3].texture = IMG_LoadTexture(renderer, "wall4.png");  
*/    
    // bool util and event
    _Bool isRun = 1; 
    SDL_Event event; 

    // init Map
    //initMap(map); 
    // init player
    initPlayer(&Player); 


    // grafica
    minimap.x = SIZE_W - (SIZE_W / 5); // 512 
    minimap.y = 0; 
    minimap.w = SIZE_W / 5; // 128
    minimap.h = SIZE_H / 5; // 96
    // grafica
    player.x = Player.pos_x; 
    player.y = Player.pos_y;
    player.w = minimap.w / 8; // 16 
    player.h = minimap.h / 6; // 16

    // game loop
    while(isRun){
        // manage inputs
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT: 
                    isRun = 0; 
                    break; 
                case SDL_KEYDOWN: 
                    switch (event.key.keysym.sym){
                        case SDLK_ESCAPE: 
                            isRun = 0; 
                            break; 

                        case SDLK_w: 
                        case SDLK_UP:
                            Player.movingForward = 1; 
                            break; 

                        case SDLK_s: 
                        case SDLK_DOWN: 
                            Player.movingBackward = 1; 
                            break; 

                        case SDLK_a: 
                        case SDLK_LEFT:
                            Player.movingLeft = 1; 
                            break; 

                        case SDLK_d: 
                        case SDLK_RIGHT:
                            Player.movingRight = 1; 
                            break;
                            
                        default: 
                            break; 
                    }
                    break;         
                case SDL_KEYUP:
                    switch(event.key.keysym.sym){
                        case SDLK_w: 
                        case SDLK_UP: 
                            Player.movingForward = 0; 
                            break; 
                        case SDLK_s: 
                        case SDLK_DOWN:
                            Player.movingBackward = 0; 
                            break; 
                        case SDLK_a: 
                        case SDLK_LEFT:
                            Player.movingLeft = 0; 
                            break; 
                        case SDLK_d: 
                        case SDLK_RIGHT: 
                            Player.movingRight = 0; 
                            break; 
                        default:
                            break; 
                    }
                
                default: 
                    break; 
            }
        }   
        
        // background clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer); 

        //floor and ceiling
        SDL_Rect ceiling = {0, 0, SIZE_W - (SIZE_W / 5), SIZE_H / 2};
        SDL_Rect floor = {0, SIZE_H / 2, SIZE_W - (SIZE_W / 5), SIZE_H / 2};
        
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255); 
        SDL_RenderFillRect(renderer, &ceiling); 
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255); 
        SDL_RenderFillRect(renderer, &floor); 

        

        // map 
        //SDL_RenderFillRect(renderer, &map2d); 
        //drawMap(map, map2d, renderer, 60, 60, 0, 0); 

        // minimap
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderFillRect(renderer, &minimap); 
        drawMap(map, minimap, renderer, minimap.h / 8, minimap.h / 8, minimap.x, minimap.y);
        
        // RAYCASTING
        castRays(renderer, &Player, &player, map, &Ray, 1); 

        // MOVING LOGIC
        updatePlayerPosition(&Player, &player, map);
        drawPlayer(renderer, &Player, player); 
           
    
        // present render
        SDL_RenderPresent(renderer); 
    }
    


    // clean all
    /* Nel cleanup
    for(int i = 0; i < 4; i++) {
        SDL_DestroyTexture(wallTextures[i].texture);
    }
    IMG_Quit();
    */
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    SDL_Quit(); 
    
    return 0; 
}