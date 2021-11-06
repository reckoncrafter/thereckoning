#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define SYNC_POS() grid_cursor.x = avatar.position.x * grid_cell_size; grid_cursor.y = avatar.position.y * grid_cell_size;
#define GRID_HEIGHT 25
#define GRID_WIDTH 25
#define GRID_CELL_SIZE 72

#include "item_ids.h"
#include "item.h"
#include "map.h"

class Player{
public:
    SDL_Texture* player_texture;
    point position;
    Item inventory[10];

    Player(){
        for(auto i : inventory){
            i.id = 0;
        }
        position = {2,2};
        player_texture = NULL;
    }
};



class Root{
public:
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_bool mouse_active = SDL_FALSE;
    SDL_bool mouse_hover = SDL_FALSE;

    bool Running;
    SDL_Surface* Surf_Display;

    int grid_cell_size = GRID_CELL_SIZE;
    int grid_width = GRID_HEIGHT;
    int grid_height = GRID_WIDTH;
    
    int grid_size = grid_width * grid_height;

    // + 1 so that the last grid lines fit in the screen.
    int window_width = (grid_width * grid_cell_size) + 1;
    int window_height = (grid_height * grid_cell_size) + 1;

    SDL_Rect grid_cursor = {
        .w = grid_cell_size,
        .h = grid_cell_size,
    };

    SDL_Rect Inventory_Menu;

    SDL_Rect grid_cursor_ghost{0,0,grid_cell_size, grid_cell_size};

    SDL_Color grid_background = {22, 22, 22, 255}; // Barely Black
    SDL_Color grid_line_color = {44, 44, 44, 255}; // Dark grey
    // SDL_Color grid_cursor_color = {255, 255, 255, 255}; // White
    SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
    SDL_Color grid_cursor_color = grid_background;

    Player avatar;

    World init_world;
    Map* current_map = &init_world.maps[init_world.bb];

    int map_editor_current_selection = ITEM_AIR;

    // Textures
    SDL_Texture* chest_texture;
    SDL_Texture* totem_texture;
    SDL_Texture* wall_texture;
    SDL_Texture* airwall_texture;
    SDL_Texture* chef_texture;
    SDL_Texture* bunny_texture;
    SDL_Texture* ps_texture;

public:

    Root() {
        Surf_Display = NULL;
        Running = true;
    }

    void AssignTextures(Item &i){
        switch(i.id){
                case ITEM_AIR:
                    break;
                case ITEM_WALL:
                    i.item_texture = wall_texture;
                    break;
                case ITEM_AIRWALL:
                    i.item_texture = airwall_texture;
                    break;
                case ITEM_CHEST:
                    i.item_texture = chest_texture;
                    break;
                case ITEM_TOTEM:
                    i.item_texture = totem_texture;
                    break;
                case ITEM_CHEF:
                    i.item_texture = chef_texture;
                    break;
                case ITEM_BUNNY:
                    i.item_texture = bunny_texture;
                    break;
                case ITEM_PS:
                    i.item_texture = ps_texture;
                    break;
                default:
                    break;
        }
    }

    void SetItemPosition(Item &i, point pos){
        i.position = pos;
        i.rect.x = pos.x * grid_cell_size;
        i.rect.y = pos.y * grid_cell_size;
    }
    
    void InitItems(){
        for(auto &i : current_map->item_list){
            i.rect.w = grid_cell_size;
            i.rect.h = grid_cell_size;
            SetItemPosition(i, i.position);
        }
        for(auto &i : current_map->item_list){
            AssignTextures(i);
        }
    }

    void PlaceItem(int id, point pos){
        Item newItem;
        newItem.id = id;
        newItem.position = pos;
        current_map->item_list.push_back(newItem);
        InitItems();
    }

    bool OnInit(){
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
            return false;
        }
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                SDL_GetError());
            return EXIT_FAILURE;
        }
        if (SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer) < 0){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s", SDL_GetError());
            return EXIT_FAILURE;
        }
        SDL_SetWindowTitle(window, "The Reckoning");
        
        Inventory_Menu.h = (grid_cell_size * 5) + 40;
        Inventory_Menu.w = (grid_cell_size * 12) + 40;
        Inventory_Menu.x = (grid_cell_size * 4) - 20;
        Inventory_Menu.y = (grid_cell_size * 9) - 20;

        grid_cursor.x = avatar.position.x * grid_cell_size;
        grid_cursor.y = avatar.position.y * grid_cell_size;

        return true;
    }

    bool InitTextures(){

#define CHK_TXT() if(!bmp_surf) return false;

        SDL_Surface* bmp_surf = SDL_LoadBMP("textures/cat.bmp");
        // Player Texture
        CHK_TXT();

        avatar.player_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_CHEST
        bmp_surf = SDL_LoadBMP("textures/chest.bmp");
        CHK_TXT();

        chest_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_TOTEM
        bmp_surf = SDL_LoadBMP("textures/totem.bmp");
        CHK_TXT();

        totem_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_WALL
        bmp_surf = SDL_LoadBMP("textures/wall.bmp");
        CHK_TXT();

        wall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_AIRWALL
        bmp_surf = SDL_LoadBMP("textures/airwall.bmp");
        CHK_TXT();

        airwall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_CHEF
        bmp_surf = SDL_LoadBMP("textures/griller.bmp");
        CHK_TXT();

        chef_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_BUNNY
        bmp_surf = SDL_LoadBMP("textures/bunny.bmp");
        CHK_TXT();

        bunny_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_PS
        bmp_surf = SDL_LoadBMP("textures/pissy_shitties.bmp");
        CHK_TXT();

        ps_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        return true;
    }

    void OnEvent(SDL_Event* Event){
        if(Event->type == SDL_QUIT) {
            Running = false;
        }
            //printf(" %i,%i ", grid_cursor.x, grid_cursor.y);
            switch (Event->type) {
            case SDL_KEYDOWN:
                switch (Event->key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    if(grid_cursor.y-grid_cell_size != (0 - grid_cell_size) ){
                        avatar.position.y -= 1;
                        grid_cursor.y -= grid_cell_size;
                    }
                    else if(current_map->GetNextMap('u') != nullptr){
                        current_map = current_map->GetNextMap('u');
                        avatar.position.y = grid_height - 1;
                        SYNC_POS();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if(grid_cursor.y+grid_cell_size != (grid_height * grid_cell_size) ){
                        avatar.position.y += 1;
                        grid_cursor.y += grid_cell_size;
                    }
                    else if(current_map->GetNextMap('d') != nullptr){
                        current_map = current_map->GetNextMap('d');
                        avatar.position.y = 0;
                        SYNC_POS();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    if(grid_cursor.x-grid_cell_size != 0 - grid_cell_size){
                        avatar.position.x -= 1;
                        grid_cursor.x -= grid_cell_size;
                    }
                    else if(current_map->GetNextMap('l') != nullptr){
                        current_map = current_map->GetNextMap('l');
                        avatar.position.x = grid_width - 1;
                        SYNC_POS();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    if(grid_cursor.x+grid_cell_size != (grid_width * grid_cell_size) ){
                        avatar.position.x += 1;
                        grid_cursor.x += grid_cell_size;
                    }
                    else if(current_map->GetNextMap('r') != nullptr){
                        current_map = current_map->GetNextMap('r');
                        avatar.position.x = 0;
                        SYNC_POS();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_0 ... SDLK_9:
                    std::cout << "set to " << Event->key.keysym.sym-SDLK_0 << std::endl;
                    map_editor_current_selection = (Event->key.keysym.sym-SDLK_0);
                    break;
                case SDLK_p:
                    for(auto i : current_map->item_list){
                        std::cout << "id: " << i.id << std::endl;
                        std::cout << "pos: " << i.position.x << ", " << i.position.y << std::endl;
                        std::cout << std::endl;
                    }
                    break;
                case SDLK_o:
                    if(!init_world.FileSave()){
                        std::cout << "save failed!" << std::endl;
                    }
                    break;
                case SDLK_l:
                    if(!init_world.FileLoad()){
                        std::cout << "load failed!" << std::endl;
                    }
                    else{
                        InitItems();
                    }
                }

                break;

            case SDL_WINDOWEVENT:
                if (Event->window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
                    mouse_hover = SDL_TRUE;
                else if (Event->window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover)
                    mouse_hover = SDL_FALSE;
                break;
            case SDL_MOUSEMOTION:
                grid_cursor_ghost.x = (Event->motion.x / grid_cell_size) * grid_cell_size;
                grid_cursor_ghost.y = (Event->motion.y / grid_cell_size) * grid_cell_size;

                if (!mouse_active)
                    mouse_active = SDL_TRUE;
                break;
            case SDL_MOUSEBUTTONDOWN:
                point placedown = {(Event->motion.x / grid_cell_size), 
                                   (Event->motion.y / grid_cell_size)};
                if(map_editor_current_selection == ITEM_AIR){
                    for(int i = 0; i < current_map->item_list.size(); i++){
                        if(current_map->item_list.at(i).position == placedown){
                            current_map->item_list.erase(current_map->item_list.begin() + i);
                        }
                    }
                }
                else{
                    for(int i = 0; i < current_map->item_list.size(); i++){
                        if(current_map->item_list.at(i).id == map_editor_current_selection 
                        && current_map->item_list.at(i).position == placedown){
                            break;
                        }
                        if(current_map->item_list.at(i).position == placedown){
                            current_map->item_list.erase(current_map->item_list.begin() + i);
                        }
                    }
                    PlaceItem(map_editor_current_selection, placedown);
                }
            break;
            }
    }

    void OnLoop(){
        
    }

    int OnItem(){
        for(auto &i : current_map->item_list){
            if(avatar.position == i.position){
                return i.id;
            }
        }
        return 0;
    }

    void OnRender(){
        // Draw grid background.
        SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g,
            grid_background.b, grid_background.a);
        SDL_RenderClear(renderer);

        // Draw grid lines.
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                               grid_line_color.b, grid_line_color.a);

        for (int x = 0; x < 1 + grid_width * grid_cell_size; x += grid_cell_size){
            SDL_RenderDrawLine(renderer, x, 0, x, window_height);
        }

        for (int y = 0; y < 1 + grid_height * grid_cell_size; y += grid_cell_size){
            SDL_RenderDrawLine(renderer, 0, y, window_width, y);
        }
        
        
        for(auto &i : current_map->item_list){
            if(i.item_texture)
            SDL_RenderCopy(renderer, i.item_texture, NULL, &i.rect);
        }

        if (mouse_active && mouse_hover) {
            SDL_SetRenderDrawColor(renderer, grid_cursor_ghost_color.r,
                                   grid_cursor_ghost_color.g,
                                   grid_cursor_ghost_color.b,
                                   grid_cursor_ghost_color.a);
            SDL_RenderFillRect(renderer, &grid_cursor_ghost);
        }
        

        SDL_SetRenderDrawColor(renderer, grid_cursor_color.r,
                               grid_cursor_color.g, grid_cursor_color.b,
                               grid_cursor_color.a);
        //SDL_RenderFillRect(renderer, &grid_cursor);

        if(OnItem() == ITEM_CHEST){
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
            SDL_RenderFillRect(renderer, &Inventory_Menu);
        }
        
        SDL_RenderCopy(renderer, avatar.player_texture, NULL, &grid_cursor);

        SDL_RenderPresent(renderer);
    }

    void OnCleanup(){
        SDL_Quit();
    }
};

