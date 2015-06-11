#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/gui/gui.hh>
#include <gazebo/util/util.hh>

#include <iostream>


void loadSystemPlugins()
{
    /*
    std::vector<std::string> pp = this->vm["server-plugin"].as<std::vector<std::string> >();

    for (std::vector<std::string>::iterator iter = pp.begin();
         iter != pp.end(); ++iter)
    {
      gazebo::addPlugin(*iter);
    }
    */
}


void playLogFile()
{
    /*
    gazebo::util::LogPlay::Instance()->Open(this->vm["play"].as<std::string>());

    std::string sdfString;
    gazebo::util::LogPlay::Instance()->Step(sdfString);

     // Load the server
    if (!gazebo::Server::LoadString(sdfString))
      return false;
    */
}


void playEmptyFile()
{
    gazebo::physics::WorldPtr world = gazebo::loadWorld("worlds/empty.world");

    while(true)
    {
      gazebo::runWorld(world, 100);
    }
}


/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
    gzLogInit("server-", "server.log");

    gazebo::Server *server = NULL;

    try
    {
      gzLogInit("server-", "server.log");

      //gazebo::util::LogRecord::Instance()->Init("server");

      server = new gazebo::Server();

      if (!server->ParseArgs(_argc, _argv))
        return -1;

      server->Run();
      server->Fini();

      delete server;
    }
    catch(gazebo::common::Exception &_e)
    {
      _e.Print();

      server->Fini();
      delete server;
      return -1;
    }

    return 0;
}
