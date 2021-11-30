#ifndef ITEM_H
#define ITEM_H

class Item{
public:
    int id;
    SDL_Rect rect;
    SDL_Texture* item_texture;
    point pos;

    Item();
};

#endif