#include "MainWindow.h"
#include <random>
#include <fstream>
#include <wx/filedlg.h>
#include <wx/datetime.h>

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_TIMER(wxID_ANY, MainWindow::OnTimer)
EVT_KEY_DOWN(MainWindow::OnKeyDown)
EVT_MENU(wxID_EXIT, MainWindow::OnExit)
EVT_MENU(ID_SETTINGS, MainWindow::OnSettings)
EVT_MENU(ID_RESET_SETTINGS, MainWindow::OnResetSettings)
EVT_MENU(ID_SAVE_BOARD, MainWindow::OnSaveBoard)
EVT_MENU(ID_LOAD_BOARD, MainWindow::OnLoadBoard)
EVT_MENU(ID_IMPORT_BOARD, MainWindow::OnImportBoard)
EVT_MENU(ID_PLAY, MainWindow::OnPlay)
EVT_MENU(ID_PAUSE, MainWindow::OnPause)
EVT_MENU(ID_NEXT, MainWindow::OnNext)
EVT_MENU(ID_RANDOM, MainWindow::OnRandom)
EVT_MENU(ID_CLEAR, MainWindow::OnClear)
wxEND_EVENT_TABLE()

MainWindow::MainWindow()
    : wxFrame(nullptr, wxID_ANY, "Snake"), timer(this)
{
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_SAVE_BOARD, "Save Board");
    fileMenu->Append(ID_LOAD_BOARD, "Load Board");
    fileMenu->Append(ID_IMPORT_BOARD, "Import Board");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "Exit");

    wxMenu* editMenu = new wxMenu();
    editMenu->Append(ID_SETTINGS, "Settings");
    editMenu->Append(ID_RESET_SETTINGS, "Reset Settings");
    editMenu->Append(ID_RANDOM, "Randomize");
    editMenu->Append(ID_CLEAR, "Clear");

    wxMenu* controlMenu = new wxMenu();
    controlMenu->Append(ID_PLAY, "Play");
    controlMenu->Append(ID_PAUSE, "Pause");
    controlMenu->Append(ID_NEXT, "Next");

    wxMenuBar* bar = new wxMenuBar();
    bar->Append(fileMenu, "File");
    bar->Append(editMenu, "Edit");
    bar->Append(controlMenu, "Controls");
    SetMenuBar(bar);

    settings.LoadSettings();
    drawingPanel = new DrawingPanel(this, &settings);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(drawingPanel, 1, wxEXPAND);
    SetSizer(sizer);

    InitializeGrid();
    drawingPanel->SetGrid(&grid);

    score = 0;
    paused = true;
    direction = Direction::Right;
    startTime = wxDateTime::Now();

    StartGame(); // <-- timer now starts inside StartGame
}

MainWindow::~MainWindow()
{
    settings.SaveSettings();
}

void MainWindow::InitializeGrid()
{
    grid.assign(settings.gridSize, std::vector<Cell>(settings.gridSize));
}

void MainWindow::StartGame()
{
    InitializeGrid();
    snake.clear();
    score = 0;
    paused = false;
    direction = Direction::Right;

    int mid = settings.gridSize / 2;
    snake.push_front({ mid, mid });
    snake.push_back({ mid - 1, mid });

    grid[mid][mid].type = CellType::SnakeHead;
    grid[mid][mid - 1].type = CellType::SnakeBody;

    AddWalls();
    SpawnFood();

    drawingPanel->Refresh();
    drawingPanel->SetFocus();        // <-- ensure keyboard input works
    timer.Start(settings.interval);   // <-- ensure snake moves automatically
}

void MainWindow::AddWalls()
{
    int max = settings.gridSize - 1;
    for (int i = 0; i < settings.gridSize; i++)
    {
        grid[0][i].type = CellType::Wall;
        grid[max][i].type = CellType::Wall;
        grid[i][0].type = CellType::Wall;
        grid[i][max].type = CellType::Wall;
    }
}

void MainWindow::SpawnFood()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> d(1, settings.gridSize - 2);
    do
    {
        food = { d(gen), d(gen) };
    } while (grid[food.y][food.x].type != CellType::Empty);
    grid[food.y][food.x].type = CellType::Food;
}

void MainWindow::MoveSnake()
{
    if (paused) return;
    wxPoint next = snake.front();
    switch (direction)
    {
    case Direction::Up: next.y--; break;
    case Direction::Down: next.y++; break;
    case Direction::Left: next.x--; break;
    case Direction::Right: next.x++; break;
    }

    auto hit = grid[next.y][next.x].type;
    if (hit == CellType::Wall || hit == CellType::SnakeBody)
    {
        StartGame();
        return;
    }

    bool ate = hit == CellType::Food;
    snake.push_front(next);
    grid[next.y][next.x].type = CellType::SnakeHead;
    grid[snake[1].y][snake[1].x].type = CellType::SnakeBody;

    if (ate)
    {
        score += 10;
        SpawnFood();
    }
    else
    {
        auto tail = snake.back();
        snake.pop_back();
        grid[tail.y][tail.x].type = CellType::Empty;
    }
}

void MainWindow::OnTimer(wxTimerEvent&)
{
    MoveSnake();
    drawingPanel->Refresh();
}

void MainWindow::OnKeyDown(wxKeyEvent& e)
{
    switch (e.GetKeyCode())
    {
    case WXK_UP:
    case 'W': if (direction != Direction::Down) direction = Direction::Up; break;
    case WXK_DOWN:
    case 'S': if (direction != Direction::Up) direction = Direction::Down; break;
    case WXK_LEFT:
    case 'A': if (direction != Direction::Right) direction = Direction::Left; break;
    case WXK_RIGHT:
    case 'D': if (direction != Direction::Left) direction = Direction::Right; break;
    case WXK_SPACE: paused = !paused; break;
    }
}

void MainWindow::SaveGame()
{
    wxFileDialog dlg(this, "Save Snake Game", "", "", "Snake files (*.snake)|*.snake", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    std::ofstream f(dlg.GetPath().ToStdString(), std::ios::binary);
    int sz = snake.size();
    f.write((char*)&sz, sizeof(sz));
    for (auto& p : snake) f.write((char*)&p, sizeof(wxPoint));
}

void MainWindow::LoadGame()
{
    wxFileDialog dlg(this, "Load Snake Game", "", "", "Snake files (*.snake)|*.snake", wxFD_OPEN);
    if (dlg.ShowModal() != wxID_OK) return;
    std::ifstream f(dlg.GetPath().ToStdString(), std::ios::binary);
    snake.clear();
    InitializeGrid();
    int sz;
    f.read((char*)&sz, sizeof(sz));
    for (int i = 0; i < sz; i++)
    {
        wxPoint p;
        f.read((char*)&p, sizeof(wxPoint));
        snake.push_back(p);
    }
    for (int i = 0; i < snake.size(); i++)
        grid[snake[i].y][snake[i].x].type =
        i == 0 ? CellType::SnakeHead : CellType::SnakeBody;
    AddWalls();
    SpawnFood();
    drawingPanel->Refresh();
}

void MainWindow::RandomizeGame() { StartGame(); }

void MainWindow::OnExit(wxCommandEvent&) { Close(); }
void MainWindow::OnSettings(wxCommandEvent&) { SettingsDialog dlg(this, &settings); dlg.ShowModal(); StartGame(); }
void MainWindow::OnResetSettings(wxCommandEvent&) { settings.ResetToDefault(); StartGame(); }
void MainWindow::OnSaveBoard(wxCommandEvent&) { SaveGame(); }
void MainWindow::OnLoadBoard(wxCommandEvent&) { LoadGame(); }
void MainWindow::OnImportBoard(wxCommandEvent&) { LoadGame(); }
void MainWindow::OnPlay(wxCommandEvent&) { paused = false; }
void MainWindow::OnPause(wxCommandEvent&) { paused = true; }
void MainWindow::OnNext(wxCommandEvent&) { MoveSnake(); drawingPanel->Refresh(); }
void MainWindow::OnRandom(wxCommandEvent&) { RandomizeGame(); }
void MainWindow::OnClear(wxCommandEvent&) { StartGame(); }
