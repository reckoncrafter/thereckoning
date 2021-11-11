#include "headers/main.h"

#define GAME_SPEED_LIMITER 16666 // 60 FPS

using namespace std;

int main(){
    srand(time(NULL));
    Root Game;

    if(Game.OnInit() == false) {
        return -1;
    }
    
    if(!Game.InitTextures())
        exit(EXIT_FAILURE); 
    
    Game.init_world.PlaceTestItems();
    Game.InitItems();
    
    /* CHECKING IF ITEMS PLACED CORRECTLY
    cout << Game.current_map->item_list.at(0).id << endl;
    cout << Game.current_map->item_list.at(0).position.x << ", " <<  Game.current_map->item_list.at(0).position.y << endl;
    cout << Game.current_map->item_list.at(0).rect.h << " x " << Game.current_map->item_list.at(0).rect.w << endl;
    cout << (Game.current_map->item_list.at(0).item_texture) << endl;
    */

    Item handItem;
    Game.avatar.Hand = &handItem;
    handItem.id = ITEM_PS;

    int _t = time(NULL);

    SDL_Event Event;
    while(Game.Running) {
        usleep(GAME_SPEED_LIMITER);
        
        while(SDL_PollEvent(&Event)) {
            if(!Game.avatar.isDead)
                Game.OnEvent(&Event);
        }

        // THIS BLOCK EXECUTES EVERY SECOND
        if(_t != time(NULL)){

            for(auto &_enemy : Game.enemies){
                _enemy.Walk();
            }
            _t = time(NULL);

            if(Game.avatar.isDead){
                Game.avatar.isDead = false;
                Game.current_map = &Game.init_world.maps[Game.init_world.bb];

                Game.wall_boundary_texture = Game.RenderWallEdges(Game.current_map->item_list);

                Game.avatar.position = {2,2};
                Game.avatar.sync();
            }

            for(auto &_enemy : Game.enemies){
                if(_enemy.walk_counter == 0){
                    _enemy.doRandomWalk ^= true;
                    // XOR babeeeey
                }         
            }
            
        }

        Game.OnLoop();
        Game.OnRender();
    }
    
    Game.OnCleanup();

    return 0;
}   