#include <gazebo/gazebo.hh>
#include <gazebo/gui/gui.hh>

#include "main_window.hh"


////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  gzLogInit("client-", "client.log");
  std::cerr << "STARTING" << std::endl;

  if( !gazebo::gui::load() )
      std::cerr << "Could not create QApplication" << std::endl;

 // gazebo::GUIWindow *window = new gazebo::GUIWindow();

  // Load gazebo
  if( !gazebo::gui::run(_argc, _argv) )
    std::cerr << "Could not run QApplication" << std::endl;
}
