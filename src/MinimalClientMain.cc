#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include <QApplication>
#include "MainWindow.hh"

int g_argc = 1;
char **g_argv;
QApplication *g_app;


////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  g_argv = new char*[g_argc];

  for (int i = 0; i < g_argc; i++)
  {
    g_argv[i] = new char[strlen("gazebo")];
    snprintf(g_argv[i], strlen("gazebo"), "gazebo");
  }

  g_app = new QApplication(g_argc, g_argv);

  // USER CODE
  gazebo::GUIWindow *window = new gazebo::GUIWindow( _argc, _argv);
  window->show();

  // END USER CODE

  g_app->exec();

  gazebo::shutdown();

  return 0;
}
