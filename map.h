class Map{
private:
    Map* up_map;
    Map* down_map;
    Map* left_map;
    Map* right_map;

public:
    vector<Item> item_list;

    Map(Map* _up = nullptr, Map* _right = nullptr, Map* _down = nullptr, Map* _left = nullptr){
        up_map = _up; right_map = _right; down_map = _down; left_map = _left;
    }

};