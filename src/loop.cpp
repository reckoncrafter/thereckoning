#include "../headers/main.h"

void Root::OnLoop(){
    for(auto &_enemy : enemies){
        if(avatar.pos == _enemy.pos && current_map == _enemy.home_map){
            SDL_SetTextureColorMod(avatar.entityTexture, 255, 140, 140);
            avatar.isDead = true;
        }
    }
    //std::cout << wall_boundaries.size() << std::endl;
}

void Root::OnCleanup(){
    SDL_Quit();
}