#include "../headers/main.h"

bool Root::OnInit(){
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

    // Fonts
    Mono = TTF_OpenFont("assets/DOS.ttf", FONT_SIZE);
    if(Mono == NULL){
        return EXIT_FAILURE;
    }

    initPauseMenu();
    return true;
}

bool Root::InitTextures(){
    #define CHK_TXT() if(!bmp_surf) return false;

    SDL_Surface* bmp_surf = SDL_LoadBMP("assets/textures/ghost.bmp");
    // Player Texture
    CHK_TXT();

    avatar.entityTexture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    // World Texture
    bmp_surf = SDL_LoadBMP("assets/textures/world.bmp");
    CHK_TXT();

    world_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    SDL_SetTextureBlendMode(world_texture, SDL_BLENDMODE_NONE);

    // ITEM_CHEST
    bmp_surf = SDL_LoadBMP("assets/textures/placeholder.bmp"); // CHANGE
    CHK_TXT();

    chest_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    // ITEM_TOTEM
    bmp_surf = SDL_LoadBMP("assets/textures/totem.bmp");
    CHK_TXT();

    totem_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    // ITEM_WALL
    bmp_surf = SDL_LoadBMP("assets/textures/placeholder.bmp");
    CHK_TXT();

    wall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    airwall_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    SDL_SetTextureColorMod(airwall_texture, 255, 60, 255);

    bmp_surf = SDL_LoadBMP("assets/textures/griller.bmp");
    CHK_TXT();

    chef_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    // ITEM_BUNNY
    bmp_surf = SDL_LoadBMP("assets/textures/person.bmp");
    CHK_TXT();

    bunny_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    // ITEM_PS
    bmp_surf = SDL_LoadBMP("assets/textures/placeholder.bmp"); // change
    CHK_TXT();

    ps_texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);

    return true;
}