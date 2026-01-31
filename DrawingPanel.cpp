cpp Snake\DrawingPanel.cpp
#include "DrawingPanel.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

wxBEGIN_EVENT_TABLE(DrawingPanel, wxPanel)
    EVT_PAINT(DrawingPanel::OnPaint)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent, Settings* settings)
    : wxPanel(parent), settings(settings)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void DrawingPanel::SetGrid(const std::vector<std::vector<Cell>>* grid)
{
    this->grid = grid;
}

void DrawingPanel::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (!grid) return;

    // Create a graphics context from the DC (use the DC overload)
    wxGraphicsContext* context = wxGraphicsContext::Create(dc);
    if (!context) return;

    wxSize size = GetSize();
    int cellW = size.GetWidth() / settings->gridSize;
    int cellH = size.GetHeight() / settings->gridSize;

    for (int r = 0; r < settings->gridSize; r++)
    {
        for (int c = 0; c < settings->gridSize; c++)
        {
            wxColour col = (*grid)[r][c].alive
                ? settings->GetLivingColor()
                : settings->GetDeadColor();

            // Use wxBrush or the overload that accepts wxBrush
            context->SetBrush(wxBrush(col));

            context->DrawRectangle(
                c * cellW,
                r * cellH,
                cellW,
                cellH
            );
        }
    }

    if (settings->showHUD)
    {
        wxString text = wxString::Format(
            "Grid: %dx%d",
            settings->gridSize,
            settings->gridSize
        );

        wxFont font(wxFontInfo(16).Bold().FaceName("Segoe UI"));

        // Create a wxGraphicsFont from the wxFont and set it explicitly.
        // Some backends (GDI+) need a wxGraphicsFont stored in the context
        // (m_font) before DrawText is called.
        wxGraphicsFont gfont = context->CreateFont(font, wxRED);
        context->SetFont(gfont);

        wxLogMessage("SetFont called");

        context->DrawText(text, 10, size.GetHeight() - 30);
    }

    delete context;
}