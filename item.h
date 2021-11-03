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
    point position;

    Item(){
        item_texture = NULL;
        position = {0,0};
        id = 0;
    }
};