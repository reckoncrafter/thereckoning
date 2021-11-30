#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <random>
#include <time.h>
#include <unistd.h>
#include <json/json.h>

#define GRID_HEIGHT 25
#define GRID_WIDTH 25
#define GRID_CELL_SIZE 72
#define NUM_PAUSE_OPTIONS 5

extern const int grid_cell_size;
extern const int grid_width;
extern const int grid_height;

extern bool gamePause;

struct point{
    int x,y;
    bool operator==(point rhs);
};

#include "../headers/item.h"
#include "../headers/map.h"
#include "../headers/entity.h"
#include "../headers/messages.h"
#include "../headers/root.h"

#endif