#include <signal.h>
#include <boost/program_options.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include "gazebo/common/ModelDatabase.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/common/Plugin.hh"
#include "gazebo/common/CommonTypes.hh"
#include "gazebo/gui/MainWindow.hh"
#include "gazebo/gui/ModelRightMenu.hh"
#include "gazebo/gui/GuiIface.hh"
#include "gazebo/util/util.hh"

#include "MainWindow.hh"


#ifdef WIN32
# define HOMEDIR "HOMEPATH"
#else
# define HOMEDIR "HOME"
#endif  // WIN32


int g_argc = 1;
char **g_argv;

namespace po = boost::program_options;
po::variables_map vm;

boost::property_tree::ptree g_propTree;

using namespace gazebo;

gui::ModelRightMenu *g_modelRightMenu = NULL;

std::string g_worldname = "default";

QApplication *g_app;
gui::MainWindow *g_main_win = NULL;
rendering::UserCameraPtr g_active_camera;
bool g_fullscreen = false;

//////////////////////////////////////////////////
void print_usage()
{
  std::cerr << "gzclient -- Gazebo GUI Client\n\n";
  std::cerr << "`gzclient` [options]\n\n";
  std::cerr << "Gazebo GUI client which allows visualization and user "
    << "interaction.\n\n";
}

//////////////////////////////////////////////////
void signal_handler(int)
{
  gazebo::gui::stop();
  gazebo::shutdown();
}

//////////////////////////////////////////////////
bool parse_args(int _argc, char **_argv)
{
  po::options_description v_desc("Options");
  v_desc.add_options()
    ("version,v", "Output version information.")
    ("verbose", "Increase the messages written to the terminal.")
    ("help,h", "Produce this help message.")
    ("gui-plugin,g", po::value<std::vector<std::string> >(), "Load a plugin.");

  po::options_description desc("Options");
  desc.add(v_desc);

  try
  {
    po::store(po::command_line_parser(_argc,
          _argv).options(desc).allow_unregistered().run(), vm);
    po::notify(vm);
  } catch(boost::exception &_e)
  {
    std::cerr << "Error. Gui Invalid arguments\n";
    return false;
  }

  if (vm.count("version"))
  {
    std::cout << GAZEBO_VERSION_HEADER << std::endl;
    return false;
  }

  if (vm.count("help"))
  {
    print_usage();
    std::cerr << v_desc << "\n";
    return false;
  }

  if (vm.count("verbose"))
  {
    gazebo::printVersion();
    gazebo::common::Console::SetQuiet(false);
  }

  /// Load all the plugins specified on the command line
  if (vm.count("gui-plugin"))
  {
    std::vector<std::string> pp =
      vm["gui-plugin"].as<std::vector<std::string> >();

    for (std::vector<std::string>::iterator iter = pp.begin();
         iter != pp.end(); ++iter)
    {
      gazebo::addPlugin(*iter);
    }
  }

  return true;
}


namespace gazebo
{
  namespace gui
  {
    void set_style()
    {
      QFile file(":/style.qss");
      file.open(QFile::ReadOnly);
      QString styleSheet = QLatin1String(file.readAll());
      g_app->setStyleSheet(styleSheet);
    }

    /////////////////////////////////////////////////
    void fini()
    {
      // Cleanup model database.
      common::ModelDatabase::Instance()->Fini();

      gui::clear_active_camera();
      rendering::fini();
      fflush(stdout);
    }
  }
}


////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  gazebo::util::LogRecord::Instance()->SetBasePath("logs");
  gzLogInit("client-", "client.log");

  // Make sure the model database has started
  gazebo::common::ModelDatabase::Instance()->Start();

  if (!parse_args(_argc, _argv))
    return 1;

  if (!gazebo::setupClient(_argc, _argv))
    return 1;

  if (!gazebo::gui::load())
    return 1;

  gazebo::gui::init();

  // Now that we're about to run, install a signal handler to allow for
  // graceful shutdown on Ctrl-C.
  struct sigaction sigact;
  sigact.sa_handler = signal_handler;
  if (sigaction(SIGINT, &sigact, NULL))
  {
    std::cerr << "signal(2) failed while setting up for SIGINT" << std::endl;
    return 1;
  }


  // USER CODE
  gazebo::GUIWindow *window = new gazebo::GUIWindow(_argc,_argv);
  window->show();

  // END USER CODE

  g_app->exec();

  gazebo::gui::fini();
  gazebo::shutdown();

  return 0;
}
