#include "../headers/main.h"

int Colliders(const std::string steps, point ref_pos, std::vector<Item> &item_list){
    int _x = 0;
    int _y = 0;
    for(auto i : steps){
        switch(i){
            case 'u':
                _y--;
                break;
            case 'd':
                _y++;
                break;
            case 'l':
                _x--;
                break;
            case 'r':
                _x++;
                break;
            default:
                break;
        }
    }
    for(auto i : item_list){
        if(i.pos.x == ref_pos.x + _x && i.pos.y == ref_pos.y + _y){
            return i.id;
        }
    }
    return 0;
}

void Entity::sync(){
    entityRect.x = pos.x * grid_cell_size;
    entityRect.y = pos.y * grid_cell_size;
}

bool Entity::EntityinRange(Entity &e, int r){
    if(abs(e.pos.x - pos.x) < r && abs(e.pos.y - pos.y) < r){
        return true;
    }
    return false;
}

bool Entity::EntityCollision(std::string steps, Entity &e){
    int _x = 0;
    int _y = 0;
    for(auto i : steps){
        switch(i){
            case 'u':
                _y--;
                break;
            case 'd':
                _y++;
                break;
            case 'l':
                _x--;
                break;
            case 'r':
                _x++;
                break;
            default:
                break;
        }
    }
    if(e.pos.x == pos.x + _x && e.pos.y == pos.y + _y && e.home_map == home_map){
        return true;
    }
    return false;
}

Player::Player(){
    pos = {2,2};
    entityTexture = NULL;
    entityRect.w = grid_cell_size;
    entityRect.h = grid_cell_size;
}

Enemy::Enemy(){
    pattern = {_U, _U, _R, _R, _D, _D, _L, _L};
    pos = {0,0};
    entityRect.h = grid_cell_size;
    entityRect.w = grid_cell_size;
}

void Enemy::GoToEntity(Entity &e, std::vector<Enemy> &neighbors){
    auto cl = [this, neighbors](std::string steps){
        if(Colliders(steps, pos, home_map->item_list)){
            return true;
        }
        for(auto i : neighbors){
            if(EntityCollision(steps, i)){
                return true;
            }
        }
        return false;
    };
    // &e is to the left
    if(e.pos.x - pos.x < 0 && !cl("l")){
        pos.x--;
    }
    // &e is to the right
    if(e.pos.x - pos.x > 0 && !cl("r")){
        pos.x++;
    }
    // &e is above
    if(e.pos.y - pos.y < 0 && !cl("u")){
        pos.y--;
    }
    // &e is below
    if(e.pos.y - pos.y > 0 && !cl("d")){
        pos.y++;
    }
    sync();
}

void Enemy::Walk(){
    int walk = doRandomWalk? rand()%4 : pattern.at(walk_counter);
    switch(walk){
        case _U:
            if(!Colliders("u", pos, home_map->item_list) && pos.y-1 >= 0){
                pos.y -= 1;
            }
            walk_counter++;
            break;
        case _D:
            if(!Colliders("d", pos, home_map->item_list) && pos.y+1 < GRID_HEIGHT){
                pos.y += 1;
            }
            walk_counter++;
            break;
        case _L:
            if(!Colliders("l", pos, home_map->item_list) && pos.x-1 >= 0){
                pos.x -= 1;
            }
            walk_counter++;
            break;
        case _R:
            if(!Colliders("r", pos, home_map->item_list) && pos.x+1 < GRID_WIDTH){
                pos.x += 1;
            }
            walk_counter++;
            break;
    }
    if(walk_counter >= pattern.size()){
        walk_counter = 0;
    }
    sync();
}