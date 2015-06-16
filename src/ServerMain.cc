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
bool OpenWorld(const std::string & _filename)
{
  gzerr << "Open World is not implemented\n";
  return false;
/*
  sdf::SDFPtr sdf(new sdf::SDF);
  if (!sdf::init(sdf))
  {
    gzerr << "Unable to initialize sdf\n";
    return false;
  }

  if (!sdf::readFile(_filename, sdf))
  {
    gzerr << "Unable to read sdf file[" << _filename << "]\n";
    return false;
  }

  msgs::WorldModify worldMsg;
  worldMsg.set_world_name("default");
  worldMsg.set_remove(true);
  this->worldModPub->Publish(worldMsg);

  physics::stop_worlds();

  physics::remove_worlds();

  sensors::remove_sensors();

  gazebo::transport::clear_buffers();

  sdf::ElementPtr worldElem = sdf->root->GetElement("world");

  physics::WorldPtr world = physics::create_world();

  physics::load_world(world, worldElem);

  physics::init_world(world);

  physics::run_world(world);

  worldMsg.set_world_name("default");
  worldMsg.set_remove(false);
  worldMsg.set_create(true);
  this->worldModPub->Publish(worldMsg);
  return true;
  */
}

void OnServerMessage(ConstServerControlPtr &_msg)
{
    if(_msg->has_open_filename())
    {
        OpenWorld(_msg->open_filename());
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

      gazebo::transport::NodePtr node = gazebo::transport::NodePtr(new gazebo::transport::Node());
      node->Init("/gazebo");
      gazebo::transport::SubscriberPtr serverSub = node->Subscribe("/gazebo/server/control", &OnServerMessage);


      gazebo::util::LogRecord::Instance()->SetBasePath("logs");
      gazebo::util::LogRecord::Instance()->Init("server");

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
