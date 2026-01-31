#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <vector>
#include <deque>
#include "DrawingPanel.h"
#include "Settings.h"
#include "Cell.h"

enum class Direction { Up, Down, Left, Right };
enum class GameState
{
    Playing,
    Paused,
    GameOver
};


class MainWindow : public wxFrame
{

public:
    MainWindow();
    ~MainWindow();

protected:
    void OnTimer(wxTimerEvent&);
    void OnKeyDown(wxKeyEvent&);
    void OnExit(wxCommandEvent&);
    void OnSettings(wxCommandEvent&);
    void OnResetSettings(wxCommandEvent&);
    void OnPlay(wxCommandEvent&);
    void OnPause(wxCommandEvent&);
    void OnNext(wxCommandEvent&);
    void OnNew(wxCommandEvent&);
    void OnSave(wxCommandEvent&);
    void OnLoad(wxCommandEvent&);

private:
    wxString currentFilePath;
    void LoadSettings();
    void SaveSettings();
    void InitializeGrid();
    void StartGame();
    void MoveSnake();
    void SpawnFood();
    void UpdateStatus();
	void Load(wxCommandEvent&);
	void SaveAs(wxCommandEvent&);
	void Save(wxCommandEvent&);
    bool IsOpposite(Direction a, Direction b) const;

    Settings settings;
    DrawingPanel* drawingPanel = nullptr;
    std::vector<std::vector<Cell>> grid;
    std::deque<wxPoint> snake;
    wxPoint food;

    Direction direction = Direction::Right;
    int score = 0;
    GameState gameState = GameState::Playing;

    wxTimer timer;
    wxStatusBar* statusBar = nullptr;

    wxDECLARE_EVENT_TABLE();
};
