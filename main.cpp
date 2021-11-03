#include "headers/main.h"

using namespace std;

int main(){
    Root Game;

    if(Game.OnInit() == false) {
        return -1;
    }
    
    
    Game.init_world.PlaceTestItems();
    Game.InitItems();
    
    /* CHECKING IF ITEMS PLACED CORRECTLY
    cout << Game.current_map->item_list.at(0).id << endl;
    cout << Game.current_map->item_list.at(0).position.x << ", " <<  Game.current_map->item_list.at(0).position.y << endl;
    cout << Game.current_map->item_list.at(0).rect.h << " x " << Game.current_map->item_list.at(0).rect.w << endl;
    cout << (Game.current_map->item_list.at(0).item_texture) << endl;
    */

    SDL_Event Event;
    while(Game.Running) {
        while(SDL_PollEvent(&Event)) {
            Game.OnEvent(&Event);
        }

        Game.OnLoop();
        Game.OnRender();
    }
    
    Game.OnCleanup();

    return 0;
}   