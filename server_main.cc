#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/gui/gui.hh>

#include <iostream>

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
    gzLogInit("server-", "server.log");

    gazebo::setupServer(_argc, _argv);
    gazebo::physics::WorldPtr world = gazebo::loadWorld("worlds/empty.world");

    while(true)
    {
      gazebo::runWorld(world, 100);
    }

    gazebo::shutdown();
}
