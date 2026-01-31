#pragma once

enum class CellState
{
    Empty,
    Snake,
    Food
};

struct Cell
{
    CellState state = CellState::Empty;
};
