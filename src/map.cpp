#include "../headers/main.h"

Map::Map(){
    up_map = nullptr;
    down_map = nullptr;
    left_map = nullptr;
    right_map = nullptr;
}

void Map::Link(Map* _up, Map* _down, Map* _left, Map* _right){
    up_map = _up;
    down_map = _down;
    left_map = _left;
    right_map = _right;
}

Map* Map::GetNextMap(char dir){
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

World::World(){
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

bool World::FileSave(){
    std::ofstream fout;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = " ";
    Json::StreamWriter* writer(builder.newStreamWriter());

    fout.open("mapfile.json");
    if(fout.fail()){return false;}

    Json::Value root;

    for(int j = 0; j < MAP_NUM; j++){
        Json::Value map;
        for(auto i : maps[j].item_list){
            Json::Value item;
            item["id"] = i.id;
            item["pos.x"] = i.pos.x;
            item["pos.y"] = i.pos.y;
            map["map_id"] = j;
            map["items"].append(item);
        }
        root["maps"].append(map);
    }

    writer->write(root, &fout);
    return true;
}

bool World::FileLoad(){
    std::ifstream fin;
    fin.open("mapfile.json");
    Json::CharReaderBuilder builder;
    Json::Value root;
    Json::String errs;
    bool r = Json::parseFromStream(builder, fin, &root, &errs);
    if(!r){
        std::cerr << "Error: " << errs;
        return false;
    }
    for(auto i : root["maps"]){
        for(auto j : i["items"]){
            Item fileItem;
            fileItem.id = j["id"].asInt();
            fileItem.pos = {j["pos.x"].asInt(), j["pos.y"].asInt()};
            maps[i["map_id"].asInt()].item_list.push_back(fileItem);
        }
    }
    return true;
}

void World::PlaceTestItems(){
    Item test_items[9];
    for(int i = 0; i < 9; i++){
        test_items[i].id = ITEM_TOTEM;
    }
    test_items[0].pos = {1, 1};
    test_items[1].pos = {GRID_WIDTH/2, 1};
    test_items[2].pos = {GRID_WIDTH - 2, 1};
    test_items[3].pos = {1, GRID_HEIGHT/2};
    test_items[4].pos = {GRID_WIDTH/2, GRID_HEIGHT/2};
    test_items[5].pos = {GRID_WIDTH - 2, GRID_HEIGHT/2};
    test_items[6].pos = {1, GRID_HEIGHT - 2};
    test_items[7].pos = {GRID_WIDTH/2, GRID_HEIGHT - 2};
    test_items[8].pos = {GRID_WIDTH - 2 ,GRID_HEIGHT - 2};

    for(int i = 0; i < MAP_NUM; i++){
        maps[i].item_list.push_back(test_items[i]);
    }
}