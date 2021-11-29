#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <random>
#include <time.h>
#include <unistd.h>
#include <json/json.h>

#define GRID_HEIGHT 25
#define GRID_WIDTH 25
#define GRID_CELL_SIZE 72
#define NUM_PAUSE_OPTIONS 5

#include "item_ids.h"
#include "item.h"
#include "map.h"
#include "messages.h"

const int grid_cell_size = GRID_CELL_SIZE;
const int grid_width = GRID_HEIGHT;
const int grid_height = GRID_WIDTH;

bool gamePause = false;

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
struct Entity{
    SDL_Texture* entityTexture;
    SDL_Rect entityRect;
    point pos;
    Map* home_map;

    void sync(){
        entityRect.x = pos.x * grid_cell_size;
        entityRect.y = pos.y * grid_cell_size;
    }
    
    // Checks if entity "e" is within range "r" of the Entity executing this funciton
    bool EntityinRange(Entity &e, int r){
        if(abs(e.pos.x - pos.x) < r && abs(e.pos.y - pos.y) < r){
            return true;
        }
        return false;
    }
    bool EntityCollision(std::string steps, Entity &e){
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
};

class Player : public Entity{
public:
    bool isDead = false;

    Player(){
        pos = {2,2};
        entityTexture = NULL;
        entityRect.w = grid_cell_size;
        entityRect.h = grid_cell_size;
    }
};

class Enemy : public Entity{
public:
    enum __Walk{ _U, _D, _L, _R };
    std::vector<__Walk> pattern;

    int walk_counter = 0;
    bool doRandomWalk = false;
    bool targetPlayerdebug = false;

    Enemy(){
        pattern = {_U, _U, _R, _R, _D, _D, _L, _L};
        pos = {0,0};
        entityRect.h = grid_cell_size;
        entityRect.w = grid_cell_size;
    }

    // Basic beeline path
    void GoToEntity(Entity &e, std::vector<Enemy> &neighbors){
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

    void Walk(){
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

};

class Root{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_bool mouse_active = SDL_FALSE;
    SDL_bool mouse_hover = SDL_FALSE;

    SDL_Surface* Surf_Display;
    
    int grid_size = grid_width * grid_height;

    // + 1 so that the last grid lines fit in the screen.
    int window_width = (grid_width * grid_cell_size) + 1;
    int window_height = (grid_height * grid_cell_size) + 1;

    SDL_Rect world_backround = {
        .x = 0,
        .y = 0,
        .w = (grid_cell_size * grid_width),
        .h = (grid_cell_size * grid_height),
    };

    SDL_Rect Dialogue_Box = {
        .w = grid_cell_size * 21,
        .h = grid_cell_size * 2,
    };
    TTF_Font* Mono;
    SDL_Surface* dialogueSurface;
    SDL_Color white = {255,255,255,255};
    SDL_Texture* dialogue_texture;

    SDL_Rect grid_cursor_ghost{0,0,grid_cell_size, grid_cell_size};

    SDL_Color grid_background = {22, 22, 22, 255}; // Barely Black
    SDL_Color grid_line_color = {44, 44, 44, 255}; // Dark grey
    // SDL_Color grid_cursor_color = {255, 255, 255, 255}; // White
    SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
    SDL_Color grid_cursor_color = grid_background;

    int map_editor_current_selection = ITEM_AIR;

    // Textures
    SDL_Texture* world_texture;
    SDL_Texture* newOptTxt;
    //SDL_Texture* wall_boundary_texture;

    SDL_Texture* chest_texture;
    SDL_Texture* totem_texture;
    SDL_Texture* wall_texture;
    SDL_Texture* airwall_texture;
    SDL_Texture* chef_texture;
    SDL_Texture* bunny_texture;
    SDL_Texture* ps_texture;

    // Message Textures
    std::vector<SDL_Texture*> msg_textures;
    SDL_Texture* current_message;

    bool render_message = false;

    // Pause Menu
    SDL_Rect Pause_Menu{
        .x = 5 * grid_cell_size,
        .y = 5 * grid_cell_size,
        .w = 15 * grid_cell_size,
        .h = 5 * grid_cell_size,
    };
    std::array<SDL_Rect, NUM_PAUSE_OPTIONS> OptionRects;
    std::array<SDL_Texture*, NUM_PAUSE_OPTIONS> OptionText;

public:
    bool Running;
    World init_world;
    std::vector<Enemy> enemies;
    Player avatar;

    Map* current_map = &init_world.maps[init_world.bb];

public:

    Root() {
        Surf_Display = NULL;
        Running = true;
    }

    void ApplyText(SDL_Texture* &txt, TTF_Font* &font, SDL_Rect &Rect, const char * str){
        SDL_Surface* tmp_surf = TTF_RenderText_Blended_Wrapped(font, str, white, Rect.w);
        if(tmp_surf == NULL){
            std::cout << "unable to render message" << std::endl;
        }
        txt = SDL_CreateTextureFromSurface(renderer, tmp_surf);
        if(txt == NULL){
            std::cout << "unable to create texture" << std::endl;
        }
        SDL_QueryTexture(txt, NULL, NULL, &Rect.w, &Rect.h);
        SDL_FreeSurface(tmp_surf);
    }

    SDL_Texture* RenderWallEdges(std::vector<Item> &item_list){
        #define RWE_FILL(__x) SDL_FillRect(wall_boundaries, &__x, SDL_MapRGBA(wall_boundaries->format, 26, 26, 26, 255));
        SDL_Surface* wall_boundaries = SDL_CreateRGBSurfaceWithFormat(0, (grid_width * grid_cell_size), (grid_height * grid_cell_size), 32, SDL_PIXELFORMAT_RGBA32);
        const int stroke = 10;

        for(auto &wall : item_list){
            if(wall.id != ITEM_WALL 
            && wall.id != ITEM_AIRWALL)
                continue;

            point origin = {wall.rect.x, wall.rect.y};
            point wp = wall.pos;
            std::vector<Item>* il = &current_map->item_list;

            if(Colliders("u", wp, *il) != wall.id){
                SDL_Rect _h = {
                    .x = origin.x,
                    .y = origin.y,
                    .w = grid_cell_size,
                    .h = stroke,
                };
                RWE_FILL(_h);
            }
            if(Colliders("d", wp, *il) != wall.id){
                SDL_Rect _h = {
                    .x = origin.x,
                    .y = origin.y + (grid_cell_size - stroke),
                    .w = grid_cell_size,
                    .h = stroke,    
                };
                RWE_FILL(_h);
            }
            if(Colliders("l", wp, *il) != wall.id){
                SDL_Rect _v = {
                    .x = origin.x,
                    .y = origin.y,
                    .w = stroke,
                    .h = grid_cell_size,
                };
                RWE_FILL(_v);
            }
            if(Colliders("r", wp, *il) != wall.id){
                SDL_Rect _v = {
                    .x = origin.x + (grid_cell_size - stroke),
                    .y = origin.y,
                    .w = stroke,                
                    .h = grid_cell_size,
                };
                RWE_FILL(_v);
            }
            
            SDL_Rect sq = {
                .w = stroke,
                .h = stroke,
            };
            if(Colliders("ul", wp, *il) != wall.id){
                sq.x = origin.x;
                sq.y = origin.y;
                RWE_FILL(sq);
            }
            if(Colliders("ur", wp, *il) != wall.id){
                sq.x = origin.x + (grid_cell_size - stroke);
                sq.y = origin.y;
                RWE_FILL(sq);
            }
            if(Colliders("dl", wp, *il) != wall.id){
                sq.x = origin.x;
                sq.y = origin.y + (grid_cell_size - stroke);
                RWE_FILL(sq);
            }
            if(Colliders("dr", wp, *il) != wall.id){
                sq.x = origin.x + (grid_cell_size - stroke);
                sq.y = origin.y + (grid_cell_size - stroke);
                RWE_FILL(sq);
            }
        }
        SDL_Texture* rtn = SDL_CreateTextureFromSurface(renderer, wall_boundaries);
        SDL_FreeSurface(wall_boundaries);
        return rtn;
    }

    void DropShadow(SDL_Texture* _tx, SDL_Rect _rc){
        
        SDL_Rect new_rc = {
            .x = _rc.x + 12,
            .y = _rc.y + 12,
            .w = _rc.w,
            .h = _rc.h,
        };
        SDL_SetTextureColorMod(_tx, 0,0,0);
        SDL_SetTextureAlphaMod(_tx, 50);
        SDL_RenderCopy(renderer, _tx, NULL, &new_rc);
        SDL_SetTextureAlphaMod(_tx, 255);
        SDL_SetTextureColorMod(_tx, 255,255,255);
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
        i.pos = pos;
        i.rect.x = pos.x * grid_cell_size;
        i.rect.y = pos.y * grid_cell_size;
    }
    
    void InitItems(){
        for(auto &i : current_map->item_list){
            i.rect.w = grid_cell_size;
            i.rect.h = grid_cell_size;
            SetItemPosition(i, i.pos);
        }
        for(auto &i : current_map->item_list){
            AssignTextures(i);
        }
        //wall_boundary_texture = RenderWallEdges(current_map->item_list);
    }

    void PlaceItem(int id, point pos){
        Item newItem;
        newItem.id = id;
        newItem.pos = pos;
        current_map->item_list.push_back(newItem);
        InitItems();
    }

    void EnemySpawn(){
        for(int i = 0; i < init_world.MAP_NUM; i++){
            for(int j = 0; j < 4; j++){
                Enemy tmp;
                tmp.pos.x = rand()%GRID_WIDTH;
                tmp.pos.y = rand()&GRID_HEIGHT;
                tmp.home_map = &init_world.maps[i];
                enemies.push_back(tmp);
            }
        }
        for(auto &_enemy : enemies){
            _enemy.sync();
            _enemy.walk_counter = rand()%8;
        }
    }

    void initPauseMenu(){
        for(int i = 0; i < NUM_PAUSE_OPTIONS; i++){
            OptionRects[i].y = Pause_Menu.y + (60*i+1) + 20;
            OptionRects[i].x = Pause_Menu.x + 10;
            OptionRects[i].w = Pause_Menu.w - 20;
            OptionRects[i].h = 50;
        }

        ApplyText(OptionText[0], Mono, OptionRects[0], "Load Map");
        ApplyText(OptionText[1], Mono, OptionRects[1], "Save Current Map");
        ApplyText(OptionText[2], Mono, OptionRects[2], "Eat Ass");
        ApplyText(OptionText[3], Mono, OptionRects[3], "Empty Map");
        ApplyText(OptionText[4], Mono, OptionRects[4], "Quit Game");
        
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
        if(TTF_Init() < 0){
            return false;
        }
        SDL_SetWindowTitle(window, "The Reckoning");

        Dialogue_Box.x = grid_cell_size * 2;
        Dialogue_Box.y = grid_cell_size * 2;


        // Players and NPCs
        avatar.sync();
        EnemySpawn();

        // Fonts
        Mono = TTF_OpenFont("DOS.ttf", 36);
        if(Mono == NULL){
            return EXIT_FAILURE;
        }

        initPauseMenu();
        return true;
    }

    bool InitTextures(){

#define CHK_TXT() if(!bmp_surf) return false;

        SDL_Surface* bmp_surf = SDL_LoadBMP("textures/ghost.bmp");
        // Player Texture
        CHK_TXT();

        avatar.entityTexture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // World Texture
        bmp_surf = SDL_LoadBMP("textures/world.bmp");
        CHK_TXT();

        world_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
        SDL_SetTextureBlendMode(world_texture, SDL_BLENDMODE_NONE);

        // ITEM_CHEST
        bmp_surf = SDL_LoadBMP("textures/placeholder.bmp"); // CHANGE
        CHK_TXT();

        chest_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_TOTEM
        bmp_surf = SDL_LoadBMP("textures/totem.bmp");
        CHK_TXT();

        totem_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_WALL
        bmp_surf = SDL_LoadBMP("textures/placeholder.bmp");
        CHK_TXT();

        wall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
        airwall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
        SDL_SetTextureColorMod(airwall_texture, 255, 60, 255);

        /*
        // ITEM_AIRWALL
        bmp_surf = SDL_LoadBMP("textures/airwall.bmp");
        CHK_TXT();

        airwall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
        */

        // ITEM_CHEF
        bmp_surf = SDL_LoadBMP("textures/griller.bmp");
        CHK_TXT();

        chef_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        // ITEM_BUNNY
        bmp_surf = SDL_LoadBMP("textures/person.bmp");
        CHK_TXT();

        bunny_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
        
            
        // this should be changed
        for(auto &_enemy : enemies){
            _enemy.entityTexture = bunny_texture;
        }

        // ITEM_PS
        bmp_surf = SDL_LoadBMP("textures/placeholder.bmp"); // change
        CHK_TXT();

        ps_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

        return true;
    }

    int OnItem(){
        for(auto &i : current_map->item_list){
            if(avatar.pos == i.pos){
                return i.id;
            }
        }
        return 0;
    }
    bool OnSpeaker(){
        switch(OnItem()){
            case ITEM_CHEF:
                return true;
            case ITEM_TOTEM:
                return true;
        }
        return false;
    }

    const char* Speak(int Item_ID){
        int r;
        switch(Item_ID){
            case ITEM_CHEF:
                r = rand()%centrist_messages_count;
                return centrist_messages[r].c_str();
                break;
            case ITEM_TOTEM:
                r = rand()%totem_messages_count;
                return totem_messages[r].c_str();
                break;
            default:
                break;
        }
        return "";
    }

    void OnEvent(SDL_Event* Event){
        SDL_Keymod mod = SDL_GetModState();
        if(Event->type == SDL_QUIT) {
            Running = false;
        }
        auto cl = [&](std::string steps){
            if(Colliders(steps, avatar.pos, current_map->item_list) == ITEM_WALL){
                return false;
            }
            if(avatar.isDead){
                return false;
            }
            return true;
        };
            //printf(" %i,%i ", grid_cursor.x, grid_cursor.y);
            switch (Event->type) {
            case SDL_KEYDOWN:
                switch (Event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    gamePause ^= true;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    if(avatar.entityRect.y-grid_cell_size != (0 - grid_cell_size)){
                        if(cl("u")){
                            avatar.pos.y -= 1;
                            avatar.sync();
                        }
                    }
                    else if(current_map->GetNextMap('u') != nullptr){
                        current_map = current_map->GetNextMap('u');
                        avatar.pos.y = grid_height - 1;
                        avatar.sync();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if(avatar.entityRect.y+grid_cell_size != (grid_height * grid_cell_size)){
                        if(cl("d")){
                            avatar.pos.y += 1;
                            avatar.entityRect.y += grid_cell_size;
                        }
                    }
                    else if(current_map->GetNextMap('d') != nullptr){
                        current_map = current_map->GetNextMap('d');
                        avatar.pos.y = 0;
                        avatar.sync();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    if(avatar.entityRect.x-grid_cell_size != 0 - grid_cell_size){
                        if(cl("l")){
                            avatar.pos.x -= 1;
                            avatar.entityRect.x -= grid_cell_size;
                        }
                    }
                    else if(current_map->GetNextMap('l') != nullptr){
                        current_map = current_map->GetNextMap('l');
                        avatar.pos.x = grid_width - 1;
                        avatar.sync();
                        InitItems();
                        SDL_SetWindowTitle(window, current_map->name.c_str());
                    }
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    if(avatar.entityRect.x+grid_cell_size != (grid_width * grid_cell_size)){
                        if(cl("r")){
                            avatar.pos.x += 1;
                            avatar.entityRect.x += grid_cell_size;
                        }
                    }
                    else if(current_map->GetNextMap('r') != nullptr){
                        current_map = current_map->GetNextMap('r');
                        avatar.pos.x = 0;
                        avatar.sync();
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
                        std::cout << "pos: " << i.pos.x << ", " << i.pos.y << std::endl;
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
                    break;
                case SDLK_r:
                    if(mod & KMOD_CTRL){
                        std::cout << "Reset.." << std::endl;
                        for(auto &e : enemies){
                            e.pos.x = rand()%GRID_WIDTH;
                            e.pos.y = rand()&GRID_HEIGHT;
                            e.sync();
                        }
                    }
                    break;
                }
                if(OnSpeaker()){
                    render_message = true;
                    const char* s = Speak(OnItem());
                    SDL_Surface* tmp_surf = TTF_RenderText_Blended_Wrapped(Mono, s, white, Dialogue_Box.w);
                    if(tmp_surf == NULL){
                        std::cout << "unable to render message" << std::endl;
                    }
                    current_message = SDL_CreateTextureFromSurface(renderer, tmp_surf);
                    SDL_FreeSurface(tmp_surf);
                }
                else{
                    render_message = false;
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
                if(gamePause){
                    SDL_Point click = {Event->motion.x, Event->motion.y};
                    if(SDL_PointInRect(&click, &OptionRects[0])){
                        std::cout << "Load Map" << std::endl;
                        // THIS WORKS
                    }
                    break;
                }

                point placedown = {(Event->motion.x / grid_cell_size), 
                                   (Event->motion.y / grid_cell_size)};
                if(map_editor_current_selection == ITEM_AIR){
                    for(int i = 0; i < current_map->item_list.size(); i++){
                        if(current_map->item_list.at(i).pos == placedown){
                            current_map->item_list.erase(current_map->item_list.begin() + i);
                        }
                    }
                }
                else{
                    for(int i = 0; i < current_map->item_list.size(); i++){
                        if(current_map->item_list.at(i).id == map_editor_current_selection 
                        && current_map->item_list.at(i).pos == placedown){
                            break;
                        }
                        if(current_map->item_list.at(i).pos == placedown){
                            current_map->item_list.erase(current_map->item_list.begin() + i);
                        }
                    }
                    PlaceItem(map_editor_current_selection, placedown);
                }
                break;
            }
    }

    void OnLoop(){
        for(auto &_enemy : enemies){
            if(avatar.pos == _enemy.pos && current_map == _enemy.home_map){
                SDL_SetTextureColorMod(avatar.entityTexture, 255, 140, 140);
                avatar.isDead = true;
            }
        }
        //std::cout << wall_boundaries.size() << std::endl;
    }

    void OnRender(){
        SDL_RenderCopy(renderer, world_texture, NULL, &world_backround);

        
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
            
        for(auto &_enemy : enemies){
            if(current_map == _enemy.home_map){
                if(_enemy.targetPlayerdebug){
                    SDL_SetTextureColorMod(_enemy.entityTexture, 255, 50, 50);
                }
                else if(_enemy.doRandomWalk){
                    SDL_SetTextureColorMod(_enemy.entityTexture, 100, 100, 255);
                }
                else{
                    SDL_SetTextureColorMod(_enemy.entityTexture, 255, 255, 255);
                }

                SDL_RenderCopy(renderer, _enemy.entityTexture, NULL, &_enemy.entityRect);
            }
        }

        if(render_message){
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
            SDL_RenderFillRect(renderer, &Dialogue_Box);
            SDL_Rect tmp = Dialogue_Box;
            tmp.x += 5;
            tmp.y += 5;
            SDL_QueryTexture(current_message, NULL, NULL, &tmp.w, &tmp.h);
            SDL_RenderCopy(renderer, current_message, NULL, &tmp);
        }

        //DropShadow(avatar.entityTexture, avatar.entityRect);

        if(avatar.isDead)
            SDL_SetTextureColorMod(avatar.entityTexture, 255,50,50);
        else{
            SDL_SetTextureColorMod(avatar.entityTexture, 255,255,255);
        }

        SDL_RenderCopy(renderer, avatar.entityTexture, NULL, &avatar.entityRect);

        if(gamePause){
            SDL_SetRenderDrawColor(renderer, 25,25,25,255);
            SDL_RenderFillRect(renderer, &Pause_Menu);
            for(int i = 0; i < NUM_PAUSE_OPTIONS; i++){
                //SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
                //SDL_RenderFillRect(renderer, &OptionRects[i]);
                SDL_RenderCopy(renderer, OptionText[i], NULL, &OptionRects[i]);
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }

    void OnCleanup(){
        SDL_Quit();
    }
};

