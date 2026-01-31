#pragma once
#include <wx/wx.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include "Settings.h"

class SettingsDialog : public wxDialog
{
public:
    SettingsDialog(wxWindow* parent, Settings& settings);

private:
    Settings& m_settings;

    wxColourPickerCtrl* snakePicker;
    wxColourPickerCtrl* foodPicker;
    wxColourPickerCtrl* gridPicker;
    wxColourPickerCtrl* bgPicker;

    wxCheckBox* showGrid;
    wxCheckBox* showHUD;

    wxSpinCtrl* gridSizeCtrl;
    wxSpinCtrl* intervalCtrl;

    void OnOK(wxCommandEvent&);
};
