#pragma once
#include <wx/wx.h>
#include <vector>
#include "Cell.h"
#include "Settings.h"

class DrawingPanel : public wxPanel
{
public:
    DrawingPanel(wxWindow* parent, Settings* settings);

    void SetGrid(const std::vector<std::vector<Cell>>* grid);

private:
    const std::vector<std::vector<Cell>>* grid = nullptr;
    Settings* settings = nullptr;

    void OnPaint(wxPaintEvent& event);

    wxDECLARE_EVENT_TABLE();
};
