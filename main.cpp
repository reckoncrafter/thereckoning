#include "main.h"
#include "map.h"

using namespace std;

int main(){
    Root Game;

    if(Game.OnInit() == false) {
        return -1;
    }
    
    
    Game.items[0].id = ITEM_CHEST;
    Game.SetItemPosition(Game.items[0], {5,5});

    Game.InitItems();
    

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