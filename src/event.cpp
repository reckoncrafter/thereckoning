#include "../headers/main.h"

void Root::OnEvent(SDL_Event* Event){
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
        if(gamePause){
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
                if(SDL_PointInRect(&click, &OptionRects[LOAD])){
                    std::cout << "Load Map..." << std::endl;
                    if(!init_world.FileLoad()){
                        std::cout << "load failed!" << std::endl;
                    }
                    else{
                        InitItems();
                    }
                }
                if(SDL_PointInRect(&click, &OptionRects[SAVE])){
                    std::cout << "Save Map..." << std::endl;
                    if(!init_world.FileSave()){
                        std::cout << "save failed!" << std::endl;
                    }
                }
                if(SDL_PointInRect(&click, &OptionRects[CLEAR])){
                    std::cout << "Destroying all items..." << std::endl;
                    for(int i = 0; i < init_world.MAP_NUM; i++){
                        init_world.maps[i].item_list.clear();
                    }
                }
                if(SDL_PointInRect(&click, &OptionRects[PACIFY])){
                    std::cout << "Vaporizing all enemies..." << std::endl;
                    enemies.clear();
                }
                if(SDL_PointInRect(&click, &OptionRects[SUMMON])){
                    std::cout << "Calling upon the Underworld..." << std::endl;
                    enemies.clear();
                    EnemySpawn();
                }
                if(SDL_PointInRect(&click, &OptionRects[QUIT])){
                    std::cout << "Quitting..." << std::endl;
                    Running = false;
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