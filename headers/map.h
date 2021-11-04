class Map{
private:
    Map* up_map;
    Map* down_map;
    Map* left_map;
    Map* right_map;

public:

    std::vector<Item> item_list;
    std::string name;

    Map(){
        up_map = nullptr;
        down_map = nullptr;
        left_map = nullptr;
        right_map = nullptr;
    }

    void Link(Map* _up, Map* _down, Map* _left, Map* _right){
        up_map = _up;
        down_map = _down;
        left_map = _left;
        right_map = _right;
    }

    Map* GetNextMap(char dir){
        switch(dir){
            case 'u':
                return up_map;
            case 'd':
                return down_map;
            case 'l':
                return left_map;
            case 'r':
                return right_map;
            default:
                return nullptr;
        }
        return nullptr;
    }

};

class World{
public:
    // World
    Map aa, ab, ac, ba, bb, bc, ca, cb, cc;

    // Linking
    World(){
        aa.Link(nullptr, &ba, nullptr, &ab); aa.name = "aa";
        ab.Link(nullptr, &bb, &aa, &ac); ab.name = "ab";
        ac.Link(nullptr, &bc, &ab, nullptr); ac.name = "ac";

        ba.Link(&aa, &ca, nullptr, &bb); ba.name = "ba";
        bb.Link(&ab, &cb, &ba, &bc); bb.name = "bb";
        bc.Link(&ac, &cc, &bb, nullptr); bc.name = "bc";

        ca.Link(&ba, nullptr, nullptr, &cb); ca.name = "ca";
        cb.Link(&bb, nullptr, &ca, &cc); cb.name = "cb";
        cc.Link(&bc, nullptr, &cb, nullptr); cc.name = "cc";
    }
    /*
        A        B        C
    A  [aa] <-> [ab] <-> [ac]
        %        %        %
    B  [ba] <-> [bb] <-> [bc]
        %        %        %
    C  [ca] <-> [cb] <-> [cc]

    */

    void PlaceTestItems(){
        Item test_items[9];
        for(int i = 0; i < 9; i++){
            test_items[i].id = ITEM_TOTEM;
        }
        test_items[0].position = {1, 1};
        test_items[1].position = {GRID_WIDTH/2, 1};
        test_items[2].position = {GRID_WIDTH - 2, 1};
        test_items[3].position = {1, GRID_HEIGHT/2};
        test_items[4].position = {GRID_WIDTH/2, GRID_HEIGHT/2};
        test_items[5].position = {GRID_WIDTH - 2, GRID_HEIGHT/2};
        test_items[6].position = {1, GRID_HEIGHT - 2};
        test_items[7].position = {GRID_WIDTH/2, GRID_HEIGHT - 2};
        test_items[8].position = {GRID_WIDTH - 2 ,GRID_HEIGHT - 2};

        aa.item_list.push_back(test_items[0]);
        ab.item_list.push_back(test_items[1]);
        ac.item_list.push_back(test_items[2]);
        ba.item_list.push_back(test_items[3]);
        bb.item_list.push_back(test_items[4]);
        bc.item_list.push_back(test_items[5]);
        ca.item_list.push_back(test_items[6]);
        cb.item_list.push_back(test_items[7]);
        cc.item_list.push_back(test_items[8]);
    }
};