#include "SettingsDialog.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/spinctrl.h>

SettingsDialog::SettingsDialog(wxWindow* parent, Settings& settings)
    : wxDialog(parent, wxID_ANY, "Settings"), m_settings(settings)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    snakePicker = new wxColourPickerCtrl(this, wxID_ANY, m_settings.SnakeColor());
    foodPicker = new wxColourPickerCtrl(this, wxID_ANY, m_settings.FoodColor());
    gridPicker = new wxColourPickerCtrl(this, wxID_ANY, m_settings.GridColor());
    bgPicker = new wxColourPickerCtrl(this, wxID_ANY, m_settings.BgColor());

    showGrid = new wxCheckBox(this, wxID_ANY, "Show Grid");
    showHUD = new wxCheckBox(this, wxID_ANY, "Show HUD");

    showGrid->SetValue(m_settings.showGrid);
    showHUD->SetValue(m_settings.showHUD);

    gridSizeCtrl = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 5, 100, m_settings.gridSize);
    intervalCtrl = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 2000, m_settings.intervalMs);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Snake Color"));
    sizer->Add(snakePicker);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Food Color"));
    sizer->Add(foodPicker);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Grid Color"));
    sizer->Add(gridPicker);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Background Color"));
    sizer->Add(bgPicker);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Grid Size"));
    sizer->Add(gridSizeCtrl);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Timer Interval (ms)"));
    sizer->Add(intervalCtrl);

    sizer->Add(showGrid);
    sizer->Add(showHUD);

    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL));
    SetSizerAndFit(sizer);

    Bind(wxEVT_BUTTON, &SettingsDialog::OnOK, this, wxID_OK);
}

void SettingsDialog::OnOK(wxCommandEvent&)
{
    auto s = snakePicker->GetColour();
    m_settings.snakeR = s.Red(); m_settings.snakeG = s.Green(); m_settings.snakeB = s.Blue();

    auto f = foodPicker->GetColour();
    m_settings.foodR = f.Red(); m_settings.foodG = f.Green(); m_settings.foodB = f.Blue();

    auto g = gridPicker->GetColour();
    m_settings.gridR = g.Red(); m_settings.gridG = g.Green(); m_settings.gridB = g.Blue();

    auto b = bgPicker->GetColour();
    m_settings.bgR = b.Red(); m_settings.bgG = b.Green(); m_settings.bgB = b.Blue();

    m_settings.showGrid = showGrid->GetValue();
    m_settings.showHUD = showHUD->GetValue();

    m_settings.gridSize = gridSizeCtrl->GetValue();
    m_settings.intervalMs = intervalCtrl->GetValue();

    m_settings.Save();
    EndModal(wxID_OK);
}
