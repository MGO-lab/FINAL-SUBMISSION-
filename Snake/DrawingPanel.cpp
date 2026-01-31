#include "DrawingPanel.h"
#include <wx/dcbuffer.h>

wxBEGIN_EVENT_TABLE(DrawingPanel, wxPanel)
EVT_PAINT(DrawingPanel::OnPaint)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent, Settings* settings)
    : wxPanel(parent), settings(settings)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    SetFocus();
    SetFocusFromKbd();
}


void DrawingPanel::SetGrid(const std::vector<std::vector<Cell>>* g)
{
    grid = g;
    Refresh();
}

void DrawingPanel::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(settings->BgColor());
    dc.Clear();

    if (!grid) return;

    int rows = grid->size();
    int cols = (*grid)[0].size();

    int w, h; GetClientSize(&w, &h);
    int cellW = (cols > 0) ? (w / cols) : w;
    int cellH = (rows > 0) ? (h / rows) : h;

    for (int y = 0;y < rows;y++)
    {
        for (int x = 0;x < cols;x++)
        {
            const Cell& cell = (*grid)[y][x];
            if (cell.state == CellState::Snake) dc.SetBrush(settings->SnakeColor());
            else if (cell.state == CellState::Food) dc.SetBrush(settings->FoodColor());
            else dc.SetBrush(settings->BgColor());

            if (settings->showGrid) dc.SetPen(settings->GridColor());
            else dc.SetPen(*wxTRANSPARENT_PEN);

            dc.DrawRectangle(x * cellW, y * cellH, cellW, cellH);
        }
    }
}
