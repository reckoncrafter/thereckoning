#include "../headers/main.h"

bool gamePause = false;
const int grid_cell_size = GRID_CELL_SIZE;
const int grid_width = GRID_HEIGHT;
const int grid_height = GRID_WIDTH;

Root::Root(){
    Surf_Display = NULL;
    Running = true;
}

void Root::ApplyText(SDL_Texture* &txt, TTF_Font* &font, SDL_Rect &Rect, const char * str){
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

void Root::AssignTextures(Item &i){
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

void Root::SetItemPosition(Item &i, point pos){
    i.pos = pos;
    i.rect.x = pos.x * grid_cell_size;
    i.rect.y = pos.y * grid_cell_size;
}

void Root::EnemySpawn(){
    for(int i = 0; i < init_world.MAP_NUM; i++){
        for(int j = 0; j < ENEMIES_PER_MAP; j++){
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
        _enemy.entityTexture = bunny_texture;
    }

}

void Root::InitItems(){
    for(auto &i : current_map->item_list){
        i.rect.w = grid_cell_size;
        i.rect.h = grid_cell_size;
        SetItemPosition(i, i.pos);
    }
    for(auto &i : current_map->item_list){
        AssignTextures(i);
    }
}

void Root::PlaceItem(int id, point pos){
    Item newItem;
    newItem.id = id;
    newItem.pos = pos;
    current_map->item_list.push_back(newItem);
    InitItems();
}

void Root::initPauseMenu(){
    for(int i = 0; i < NUM_PAUSE_OPTIONS; i++){
        OptionRects[i].y = Pause_Menu.y + (20*i+1) + 5;
        OptionRects[i].x = Pause_Menu.x + 4;
        OptionRects[i].w = Pause_Menu.w - 4;
        OptionRects[i].h = 10;
    }

    ApplyText(OptionText[LOAD], Mono, OptionRects[LOAD], "Load Map");
    ApplyText(OptionText[SAVE], Mono, OptionRects[SAVE], "Save Current Map");
    ApplyText(OptionText[CLEAR], Mono, OptionRects[CLEAR], "Clear Map");
    ApplyText(OptionText[PACIFY], Mono, OptionRects[PACIFY], "Destroy Enemies");
    ApplyText(OptionText[SUMMON], Mono, OptionRects[SUMMON], "Summon / Reset Enemies");
    ApplyText(OptionText[QUIT], Mono, OptionRects[QUIT], "Quit Game");
}

int Root::OnItem(){
    for(auto &i : current_map->item_list){
        if(avatar.pos == i.pos){
            return i.id;
        }
    }
    return 0;
}

bool Root::OnSpeaker(){
    switch(OnItem()){
        case ITEM_CHEF:
            return true;
        case ITEM_TOTEM:
            return true;
    }
    return false;
}

const char* Root::Speak(int Item_ID){
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