#include "../headers/main.h"

bool point::operator==(point rhs){
    return this->x == rhs.x && this->y == rhs.y;
}

Item::Item(){
    item_texture = NULL;
    pos = {0,0};
    id = 0;
}