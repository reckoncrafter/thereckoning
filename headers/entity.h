#ifndef ENTITY_H
#define ENTITY_H

int Colliders(const std::string steps, point ref_pos, std::vector<Item> &item_list);

class Entity{
public:
    SDL_Texture* entityTexture;
    SDL_Rect entityRect;
    point pos;
    Map* home_map;

    void sync();
    bool EntityinRange(Entity &e, int r);
    bool EntityCollision(std::string steps, Entity &e);
};

class Player : public Entity{
public:
    bool isDead = false;
    Player();    
};

class Enemy : public Entity{
public:
    enum __Walk{_U, _D, _L, _R};
    std::vector<__Walk> pattern;

    int walk_counter = 0;
    bool doRandomWalk = false;

    bool targetPlayerDebug = false;

    Enemy();
    void GoToEntity(Entity &e, std::vector<Enemy> &neighbors);
    void Walk();
};

#endif