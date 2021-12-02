#ifndef ENTITY_H
#define ENTITY_H

int Colliders(const std::string steps, point ref_pos, std::vector<Item> &item_list);

class Entity{
public:
    SDL_Texture* entityTexture;
    SDL_Rect entityRect;
    point pos;
    Map* home_map;
    bool isDead = false;

    Entity();

    void sync();
    bool EntityinRange(Entity &e, int r);
    bool EntityCollision(std::string steps, Entity &e);
};

class Player : public Entity{
public:
    SDL_Texture* arrowTexture;
    SDL_Rect arrowRect;
    char arrowDir = 'u';
    point arrowPos;

    void sync();

    void alignArrow(char dir);
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

class Bullet : public Entity{
public:
    bool isActive = false;
    int velocity = 1;
    char fireDirection;

    Bullet();

    bool step(char dir, std::vector<Enemy> &targets, Map* current);
    //void shoot(point origin, char dir, std::vector<Enemy> &targets);
};

#endif