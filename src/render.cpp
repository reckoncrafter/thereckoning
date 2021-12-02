#include "../headers/main.h"
void Root::OnRender(){
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
            if(_enemy.targetPlayerDebug){
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
    if(bullet.isActive){
        SDL_RenderCopy(renderer, bullet.entityTexture, NULL, &bullet.entityRect);
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
    switch(avatar.arrowDir){
        case 'u':
            SDL_RenderCopy(renderer, avatar.arrowTexture, NULL, &avatar.arrowRect);
            break;
        case 'r':
            SDL_RenderCopyEx(renderer, avatar.arrowTexture, NULL, &avatar.arrowRect, 90.0, NULL, SDL_FLIP_NONE);
            break;
        case 'd':
            SDL_RenderCopyEx(renderer, avatar.arrowTexture, NULL, &avatar.arrowRect, 180.0, NULL, SDL_FLIP_NONE);
            break;
        case 'l':
            SDL_RenderCopyEx(renderer, avatar.arrowTexture, NULL, &avatar.arrowRect, 270.0, NULL, SDL_FLIP_NONE);
            break;
    }
    

    if(gamePause){
        /*
        SDL_Rect Border{
            .x = Pause_Menu.x - 20,
            .y = Pause_Menu.y + 10,
            .w = Pause_Menu.w + 20,
            .h = Pause_Menu.h + 20
        };
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderFillRect(renderer, &Border);
        */
       
        SDL_SetRenderDrawColor(renderer, 25,25,25,255);
        SDL_RenderFillRect(renderer, &Pause_Menu);
        if (mouse_active && mouse_hover){
            SDL_Point mp;
            SDL_GetMouseState(&mp.x, &mp.y);
            for(auto &i : OptionRects){
                if(SDL_PointInRect(&mp, &i)){
                    SDL_SetRenderDrawColor(renderer, 55,55,55,255);
                    SDL_RenderFillRect(renderer, &i);
                    break;
                }
            }
        }
        for(int i = 0; i < NUM_PAUSE_OPTIONS; i++){
            //SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
            //SDL_RenderFillRect(renderer, &OptionRects[i]);
            SDL_RenderCopy(renderer, OptionText[i], NULL, &OptionRects[i]);
        }
    }
    
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}