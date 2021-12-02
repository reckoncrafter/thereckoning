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


void Player::sync(){
    Entity::sync();
    auto alg = [&](){
        arrowRect.x = arrowPos.x * grid_cell_size;
        arrowRect.y = arrowPos.y * grid_cell_size;
    };
    switch(arrowDir){
        case 'u':
            arrowPos = {pos.x, pos.y - 1};
            alg();
            break;
        case 'd':
            arrowPos = {pos.x, pos.y + 1};
            alg();
            break;
        case 'l':
            arrowPos = {pos.x - 1, pos.y};
            alg();
            break;
        case 'r':
            arrowPos = {pos.x + 1, pos.y};
            alg();
            break;
        default:
            std::cout << "Fuck" << std::endl;
    }
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

Entity::Entity(){
    entityTexture = NULL;
    entityRect.w = grid_cell_size;
    entityRect.h = grid_cell_size;
}

Player::Player(){
    pos = {2,2};
    entityTexture = NULL;
    arrowTexture = NULL;
    arrowRect.h = grid_cell_size;
    arrowRect.w = grid_cell_size;
    arrowPos = {pos.y - 1, pos.x};
}

Enemy::Enemy(){
    pattern = {_U, _U, _R, _R, _D, _D, _L, _L};
    pos = {0,0};
}

Bullet::Bullet(){
    pos = {0,0};
    sync();
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

bool Bullet::step(char dir, std::vector<Enemy> &targets, Map* current){
    auto cl = [this, &targets, current](std::string steps){
        if(Colliders(steps, pos, current->item_list)){
            return true;
        }
        for(auto &i : targets){
            if(i.home_map == current){
                if(pos == i.pos){
                    i.isDead = true;
                    return true;
                }
            }
        }
        if(pos.x < 0 || pos.x > grid_width){
            return true;
        }
        if(pos.y < 0 || pos.y > grid_height){
            return true;
        }
        return false;
    };
    switch(dir){
        case 'u':
            if(!cl("u")){
                pos.y--;
                sync();
                return true;
            }
            break;
        case 'd':
            if(!cl("d")){
                pos.y++;
                sync();
                return true;
            }
            break;
        case 'l':
            if(!cl("l")){
                pos.x--;
                sync();
                return true;
            }
            break;
        case 'r':
            if(!cl("r")){
                pos.x++;
                sync();
                return true;
            }
            break;
    }
    return false;
}
/*
void Bullet::shoot(point origin, char dir, std::vector<Enemy> &targets){
    auto cl = [this, targets](std::string steps){
        if(Colliders(steps, pos, home_map->item_list)){
            return true;
        }
        for(auto i : targets){
            if(EntityCollision(steps, i)){
                return true;
            }
        }
        return false;
    };
    this->pos = origin;
    switch(dir){
        case 'u':
            while(!cl("u")){
                pos.y--;
                usleep(GAME_SPEED_LIMITER);
            }
            break;
        case 'd':
            while(!cl("d")){
                pos.y++;
                usleep(GAME_SPEED_LIMITER);
            }
            break;
        case 'l':
            while(!cl("l")){
                pos.x--;
                usleep(GAME_SPEED_LIMITER);
            }
            break;
        case 'r':
            while(!cl("r")){
                pos.x++;
                usleep(GAME_SPEED_LIMITER);
            }
            break;
    }
}
*/