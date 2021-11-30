#ifndef ROOT_H
#define ROOT_H

enum ITEM_IDS{
    ITEM_AIR = 0,
    ITEM_WALL = 1,
    ITEM_AIRWALL = 2,
    ITEM_CHEST = 3,
    ITEM_TOTEM = 4,
    ITEM_CHEF = 5,
    ITEM_BUNNY = 6,
    ITEM_PS = 7
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
    Root();
    void ApplyText(SDL_Texture* &txt, TTF_Font* &font, SDL_Rect &Rect, const char * str);
    void AssignTextures(Item &i);
    void SetItemPosition(Item &i, point pos);
    void InitItems();
    void PlaceItem(int id, point pos);
    void EnemySpawn();
    void initPauseMenu();

    bool OnInit();

    bool InitTextures();

    int OnItem();
    bool OnSpeaker();
    const char* Speak(int Item_ID);

    void OnEvent(SDL_Event* Event);

    void OnLoop();

    void OnRender();

    void OnCleanup();
};

#endif