struct point{
    int x,y;
    bool operator==(point rhs){
        return this->x == rhs.x && this->y == rhs.y;
    }
};

class Item{
public:
    int id;
    SDL_Rect rect;
    SDL_Texture* item_texture;
    point pos;

    Item(){
        item_texture = NULL;
        pos = {0,0};
        id = 0;
    }
};