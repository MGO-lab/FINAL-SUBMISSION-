#pragma once
#include <wx/colour.h>

struct Settings
{
    bool showGrid = true;
    bool showHUD = true;

    int intervalMs = 150;
    int gridSize = 20;

    int snakeR = 0;
    int snakeG = 255;
    int snakeB = 0;

    int foodR = 255;
    int foodG = 0;
    int foodB = 0;

    int gridR = 180;
    int gridG = 180;
    int gridB = 180;

    int bgR = 0;
    int bgG = 0;
    int bgB = 0;

    wxColour SnakeColor() const { return wxColour(snakeR, snakeG, snakeB); }
    wxColour FoodColor() const { return wxColour(foodR, foodG, foodB); }
    wxColour GridColor() const { return wxColour(gridR, gridG, gridB); }
    wxColour BgColor() const { return wxColour(bgR, bgG, bgB); }

    void Load();
    void Save() const;
    void Reset();
};
