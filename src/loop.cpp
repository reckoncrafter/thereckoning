#include "../headers/main.h"

void Root::OnLoop(){
    for(auto &_enemy : enemies){
        if(avatar.pos == _enemy.pos && current_map == _enemy.home_map){
            SDL_SetTextureColorMod(avatar.entityTexture, 255, 140, 140);
            avatar.isDead = true;
        }
    }
    for(auto i = enemies.begin(); i != enemies.end(); i++){
        if(i->isDead){
            if(i->entityTexture != explosion)
            i->entityTexture = explosion;
        }
    }
    //std::cout << wall_boundaries.size() << std::endl;
    if(bullet.isActive){
        if(!bullet.step(avatar.arrowDir, enemies, current_map)){
            bullet.isActive = false;
            bullet.pos = {0,0};
        }
    }

    if(onSecond){
        for(auto i = enemies.begin(); i != enemies.end(); i++){
            if(i->isDead){
                enemies.erase(i);
            }
        }
    }
}

void Root::OnCleanup(){
    SDL_Quit();
}