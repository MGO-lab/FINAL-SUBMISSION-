#include "MainWindow.h"
#include "SettingsDialog.h"
#include <wx/filedlg.h>
#include <random>
#include "IDs.h"
#include <fstream>
#include <algorithm>

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_TIMER(wxID_ANY, MainWindow::OnTimer)
EVT_KEY_DOWN(MainWindow::OnKeyDown)
wxEND_EVENT_TABLE()

MainWindow::MainWindow()
    : wxFrame(nullptr, wxID_ANY, "Snake", wxDefaultPosition, wxSize(800, 800)),
    timer(this)
{
    settings.Load();

    drawingPanel = new DrawingPanel(this, &settings);
    SetSizer(new wxBoxSizer(wxVERTICAL));
    GetSizer()->Add(drawingPanel, 1, wxEXPAND);

    statusBar = CreateStatusBar();

    InitializeGrid();
    StartGame();
    SetFocus();

    wxMenuBar* menuBar = new wxMenuBar();

    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_NEW, "&New\tCtrl+N");
    fileMenu->Append(ID_LOAD_BOARD, "&Open\tCtrl+O");
    fileMenu->Append(ID_SAVE_BOARD, "&Save\tCtrl+S");
    fileMenu->Append(ID_SAVE_AS, "Save &As\tCtrl+Shift+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit");
    menuBar->Append(fileMenu, "&File");

    wxMenu* viewMenu = new wxMenu;
    viewMenu->Append(ID_SETTINGS, "&Settings...");
    viewMenu->Append(ID_RESET_SETTINGS, "&Reset Settings");
    menuBar->Append(viewMenu, "&View");

    wxMenu* gameMenu = new wxMenu;
    gameMenu->Append(ID_PLAY, "&Play");
    gameMenu->Append(ID_PAUSE, "&Pause");
    gameMenu->Append(ID_NEXT, "&Next");
    gameMenu->Append(ID_RANDOM, "&Randomize");
    gameMenu->Append(ID_CLEAR, "&Clear");
    menuBar->Append(gameMenu, "&Game");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MainWindow::OnSettings, this, ID_SETTINGS);
    Bind(wxEVT_MENU, &MainWindow::OnResetSettings, this, ID_RESET_SETTINGS);
    Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainWindow::OnNew, this, ID_NEW);

    Bind(wxEVT_MENU, &MainWindow::Save, this, ID_SAVE_BOARD);
    Bind(wxEVT_MENU, &MainWindow::SaveAs, this, ID_SAVE_AS);
    Bind(wxEVT_MENU, &MainWindow::Load, this, ID_LOAD_BOARD);

    Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        if (gameState != GameState::GameOver)
        {
            gameState = GameState::Playing;
            timer.Start(settings.intervalMs);
        }
        }, ID_PLAY);

    Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        if (gameState == GameState::Playing)
            gameState = GameState::Paused;
        }, ID_PAUSE);

    Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        if (gameState != GameState::GameOver) {
            GameState prev = gameState;
            gameState = GameState::Playing;
            MoveSnake();
            gameState = prev;           
        }
        }, ID_NEXT);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        std::mt19937 rng(std::random_device{}());
        int midX = rng() % settings.gridSize;
        int midY = rng() % settings.gridSize;

        snake.clear();
        snake.push_front({ midX, midY });

        int dir = rng() % 4;
        switch (dir) {
        case 0: snake.push_back({ midX - 1, midY }); direction = Direction::Right; break;
        case 1: snake.push_back({ midX + 1, midY }); direction = Direction::Left; break;
        case 2: snake.push_back({ midX, midY - 1 }); direction = Direction::Down; break;
        case 3: snake.push_back({ midX, midY + 1 }); direction = Direction::Up; break;
        }

        score = 0;
        gameState = GameState::Playing;
        SpawnFood();
        drawingPanel->Refresh();
        UpdateStatus();
        }, ID_RANDOM);

    Bind(wxEVT_MENU, [this](wxCommandEvent&) { StartGame(); }, ID_RANDOM);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { StartGame(); }, ID_CLEAR);
}

MainWindow::~MainWindow()
{
    settings.Save();
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
    direction = Direction::Right;
    gameState = GameState::Playing;

    int mid = settings.gridSize / 2;
    snake.push_front({ mid, mid });
    snake.push_back({ mid - 1, mid });

    SpawnFood();

    timer.Start(settings.intervalMs);
    drawingPanel->SetGrid(&grid);
    drawingPanel->Refresh();
    this->SetFocus();
}

void MainWindow::SpawnFood()
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, settings.gridSize - 1);

    do {
        food = { dist(rng), dist(rng) };
    } while (std::find(snake.begin(), snake.end(), food) != snake.end());

    grid[food.y][food.x].state = CellState::Food;
}

bool MainWindow::IsOpposite(Direction a, Direction b) const
{
    return (a == Direction::Up && b == Direction::Down) ||
        (a == Direction::Down && b == Direction::Up) ||
        (a == Direction::Left && b == Direction::Right) ||
        (a == Direction::Right && b == Direction::Left);
}

void MainWindow::MoveSnake()
{
    if (gameState != GameState::Playing)
        return;

    wxPoint head = snake.front();

    switch (direction)
    {
    case Direction::Up:    head.y--; break;
    case Direction::Down:  head.y++; break;
    case Direction::Left:  head.x--; break;
    case Direction::Right: head.x++; break;
    }


    if (head.x < 0 || head.y < 0 || head.x >= settings.gridSize || head.y >= settings.gridSize)
    {
        gameState = GameState::GameOver;
        timer.Stop();
        UpdateStatus();
        return;
    }


    if (std::find(snake.begin(), snake.end(), head) != snake.end())
    {
        gameState = GameState::GameOver;
        timer.Stop();
        UpdateStatus();
        return;
    }

    snake.push_front(head);

    if (head == food)
    {
        score += 10;
        SpawnFood(); 
    }
    else
    {
        auto tail = snake.back();
        grid[tail.y][tail.x].state = CellState::Empty;
        snake.pop_back();
    }

    for (auto& pt : snake)
        grid[pt.y][pt.x].state = CellState::Snake;

    grid[food.y][food.x].state = CellState::Food;

    UpdateStatus();
    drawingPanel->Refresh();
}

void MainWindow::UpdateStatus()
{
    wxString stateStr =
        (gameState == GameState::Playing) ? "Playing" :
        (gameState == GameState::Paused) ? "Paused" :
        "Game Over";

    if (settings.showHUD)
    {
        statusBar->SetStatusText(wxString::Format(
            "State: %s | Score: %d | Length: %zu",
            stateStr, score, snake.size()));
    }
    else
    {
        statusBar->SetStatusText("");
    }
}

void MainWindow::OnTimer(wxTimerEvent&)
{
    MoveSnake();
}

void MainWindow::OnKeyDown(wxKeyEvent& e)
{
    Direction newDir = direction;

    switch (e.GetKeyCode())
    {
    case WXK_UP:    newDir = Direction::Up;    break;
    case WXK_DOWN:  newDir = Direction::Down;  break;
    case WXK_LEFT:  newDir = Direction::Left;  break;
    case WXK_RIGHT: newDir = Direction::Right; break;
    case WXK_SPACE:
        if (gameState == GameState::Playing)
            gameState = GameState::Paused;
        else if (gameState == GameState::Paused)
            gameState = GameState::Playing;
        return;
    default:
        return;
    }

    if (!IsOpposite(direction, newDir))
        direction = newDir;
}

void MainWindow::OnSettings(wxCommandEvent&)
{
    SettingsDialog dlg(this, settings);
    if (dlg.ShowModal() == wxID_OK)
    {
        settings.Save();
        StartGame();
    }
}

void MainWindow::OnResetSettings(wxCommandEvent&)
{
    settings.Reset();
    StartGame();
}

void MainWindow::OnNew(wxCommandEvent&)
{
    StartGame();
}

void MainWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainWindow::Save(wxCommandEvent&)
{
    if (currentFilePath.IsEmpty())
    {
        wxCommandEvent evt;
        SaveAs(evt);
        return;
    }

    std::ofstream file(currentFilePath.ToStdString());
    if (!file.is_open()) return;

    file << settings.gridSize << "\n";
    file << snake.size() << "\n";
    for (auto& p : snake)
        file << p.x << " " << p.y << "\n";
    file << food.x << " " << food.y << "\n";
    file.close();
}

void MainWindow::SaveAs(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Save Snake Game",
        "",
        "",
        "Snake Files (*.snake)|*.snake",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    currentFilePath = dlg.GetPath();

    wxCommandEvent evt;
    Save(evt);
}

void MainWindow::Load(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Open Snake Game",
        "",
        "",
        "Snake Files (*.snake)|*.snake",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    currentFilePath = dlg.GetPath();
    std::ifstream file(currentFilePath.ToStdString());
    if (!file.is_open()) return;

    file >> settings.gridSize;
    InitializeGrid();

    size_t snakeSize;
    file >> snakeSize;

    snake.clear();
    for (size_t i = 0; i < snakeSize; ++i)
    {
        int x, y;
        file >> x >> y;
        snake.push_back({ x, y });
    }

    file >> food.x >> food.y;
    file.close();

    StartGame();
}
