#include "App.h"
#include "MainWindow.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    MainWindow* window = new MainWindow();
    window->Show(true);
    return true;
}
