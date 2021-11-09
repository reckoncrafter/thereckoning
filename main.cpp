#include "headers/main.h"

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
        while(SDL_PollEvent(&Event)) {
            if(!Game.avatar.isDead)
                Game.OnEvent(&Event);
        }

        // THIS BLOCK EXECUTES EVERY SECOND
        if(_t != time(NULL)){
            Game.enemy.Walk();
            _t = time(NULL);

            if(Game.avatar.isDead){
                Game.avatar.isDead = false;
                SDL_SetTextureColorMod(Game.avatar.player_texture, 255,255,255);
                Game.avatar.position = {2,2};
                Game.avatar.sync();
            }
            
        }

        Game.OnLoop();
        Game.OnRender();
    }
    
    Game.OnCleanup();

    return 0;
}   