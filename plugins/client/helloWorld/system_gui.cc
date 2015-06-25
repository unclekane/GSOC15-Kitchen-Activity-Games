 
#include <gazebo/math/Rand.hh>
#include <gazebo/gui/GuiIface.hh>
#include <gazebo/rendering/rendering.hh>
#include <gazebo/gazebo.hh>

namespace gazebo
{
  class SystemGUI : public SystemPlugin
  {
    /////////////////////////////////////////////
    /// \brief Destructor
    public: virtual ~SystemGUI()
    {
      this->connections.clear();
    }

    /////////////////////////////////////////////
    /// \brief Called after the plugin has been constructed.
    public: void Load(int /*_argc*/, char ** /*_argv*/)
    {
      this->connections.push_back(
          event::Events::ConnectPreRender(
            boost::bind(&SystemGUI::Update, this)));
    }

    /////////////////////////////////////////////
    // \brief Called once after Load
    private: void Init()
    {
    }

    /////////////////////////////////////////////
    /// \brief Called every PreRender event. See the Load function.
    private: void Update()
    {
        std::cout << "HELLO STD\n";
        std::cerr << "HELLO ERR\n";
    }


    /// All the event connections.
    private: std::vector<event::ConnectionPtr> connections;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_SYSTEM_PLUGIN(SystemGUI)
}
