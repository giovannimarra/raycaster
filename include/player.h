#ifndef PLAYER_H
#define PLAYER_H

struct player{
    int pos_x; 
    int pos_y; 
    float angle; 
    float speed; 
    int movingForward; 
    int movingBackward; 
    int movingLeft; 
    int movingRight;
}; 

#endif