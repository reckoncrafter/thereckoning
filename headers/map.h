#ifndef MAP_H
#define MAP_H

class Map{
private:
    Map* up_map;
    Map* down_map;
    Map* left_map;
    Map* right_map;
public:
    std::vector<Item> item_list;
    std::string name;

    Map();
    void Link(Map* _up, Map* _down, Map* _left, Map* _right);
    Map* GetNextMap(char dir);
};

class World{
public:
    #define __MAP_NUM 9
    const int MAP_NUM = __MAP_NUM;
    Map maps[__MAP_NUM];

    enum MAP_IDS{
        aa, ab, ac, ba, bb, bc, ca, cb, cc  
    };

    World();

    bool FileSave();
    bool FileLoad();

    void PlaceTestItems();
};

#endif