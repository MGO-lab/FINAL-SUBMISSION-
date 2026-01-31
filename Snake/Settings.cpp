#include "Settings.h"
#include <wx/config.h>

void Settings::Load()
{
    wxConfig cfg("Snake");

    cfg.Read("View/ShowGrid", &showGrid, true);
    cfg.Read("View/ShowHUD", &showHUD, true);

    cfg.Read("Game/IntervalMs", &intervalMs, 150);
    cfg.Read("Game/GridSize", &gridSize, 20);

    cfg.Read("Colors/SnakeR", &snakeR, 0);
    cfg.Read("Colors/SnakeG", &snakeG, 255);
    cfg.Read("Colors/SnakeB", &snakeB, 0);

    cfg.Read("Colors/FoodR", &foodR, 255);
    cfg.Read("Colors/FoodG", &foodG, 0);
    cfg.Read("Colors/FoodB", &foodB, 0);

    cfg.Read("Colors/GridR", &gridR, 180);
    cfg.Read("Colors/GridG", &gridG, 180);
    cfg.Read("Colors/GridB", &gridB, 180);

    cfg.Read("Colors/BgR", &bgR, 0);
    cfg.Read("Colors/BgG", &bgG, 0);
    cfg.Read("Colors/BgB", &bgB, 0);
}

void Settings::Save() const
{
    wxConfig cfg("Snake");

    cfg.Write("View/ShowGrid", showGrid);
    cfg.Write("View/ShowHUD", showHUD);

    cfg.Write("Game/IntervalMs", intervalMs);
    cfg.Write("Game/GridSize", gridSize);

    cfg.Write("Colors/SnakeR", snakeR);
    cfg.Write("Colors/SnakeG", snakeG);
    cfg.Write("Colors/SnakeB", snakeB);

    cfg.Write("Colors/FoodR", foodR);
    cfg.Write("Colors/FoodG", foodG);
    cfg.Write("Colors/FoodB", foodB);

    cfg.Write("Colors/GridR", gridR);
    cfg.Write("Colors/GridG", gridG);
    cfg.Write("Colors/GridB", gridB);

    cfg.Write("Colors/BgR", bgR);
    cfg.Write("Colors/BgG", bgG);
    cfg.Write("Colors/BgB", bgB);

    cfg.Flush();
}

void Settings::Reset()
{
    *this = Settings{};
}
