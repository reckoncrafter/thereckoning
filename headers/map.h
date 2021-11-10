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
#define __MAP_NUM 9
public:
    // World
    Map maps[__MAP_NUM];
    const int MAP_NUM = __MAP_NUM;

    enum MAP_IDS{
        aa, ab, ac, ba, bb, bc, ca, cb, cc
    };

    // Linking
    World(){
        maps[aa].Link(nullptr, &maps[ba], nullptr, &maps[ab]); maps[aa].name = "aa";
        maps[ab].Link(nullptr, &maps[bb], &maps[aa], &maps[ac]); maps[ab].name = "ab";
        maps[ac].Link(nullptr, &maps[bc], &maps[ab], nullptr); maps[ac].name = "ac";

        maps[ba].Link(&maps[aa], &maps[ca], nullptr, &maps[bb]); maps[ba].name = "ba";
        maps[bb].Link(&maps[ab], &maps[cb], &maps[ba], &maps[bc]); maps[bb].name = "bb";
        maps[bc].Link(&maps[ac], &maps[cc], &maps[bb], nullptr); maps[bc].name = "bc";

        maps[ca].Link(&maps[ba], nullptr, nullptr, &maps[cb]); maps[ca].name = "ca";
        maps[cb].Link(&maps[bb], nullptr, &maps[ca], &maps[cc]); maps[cb].name = "cb";
        maps[cc].Link(&maps[bc], nullptr, &maps[cb], nullptr); maps[cc].name = "cc";
    }
    /*
        A        B        C
    A  [aa] <-> [ab] <-> [ac]
        %        %        %
    B  [ba] <-> [bb] <-> [bc]
        %        %        %
    C  [ca] <-> [cb] <-> [cc]

    */

    bool FileSave(){
        std::ofstream fout;
        fout.open("mapfile.trk");
        if(fout.fail()){return false;}

        for(int j = 0; j < MAP_NUM; j++){
            for(auto i : maps[j].item_list){
                fout << j << ' ' << i.id << ' ' << i.position.x << ' ' << i.position.y;
                fout << std::endl;
            }
        }
        return true;
    }
    bool FileLoad(){
        std::ifstream fin;
        fin.open("mapfile.trk");
        if(fin.fail()){return false;}

        std::string in;
        Item fileItem;
        do{
            int mapn;

            fin >> in;
            mapn = stoi(in);

            fin >> in;
            fileItem.id = stoi(in);

            fin >> in;
            fileItem.position.x = stoi(in);

            fin >> in;
            fileItem.position.y = stoi(in);

            if(mapn < MAP_NUM){
                maps[mapn].item_list.push_back(fileItem);
                std::cout << "pushed [" << mapn << "::" << fileItem.id << "(" << fileItem.position.x << "," << fileItem.position.y << ")" << std::endl;
            }
        }while(!fin.eof());

        return true;
    }

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

        for(int i = 0; i < MAP_NUM; i++){
            maps[i].item_list.push_back(test_items[i]);
        }
    }
};