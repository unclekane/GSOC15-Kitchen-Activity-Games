#include "gazebo/common/Exception.hh"
#include "gazebo/util/LogRecord.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/Server.hh"


/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
    gzLogInit("server-", "server.log");

    gazebo::Server *server = NULL;

    try
    {
      gzLogInit("server-", "server.log");

      gazebo::util::LogRecord::Instance()->SetBasePath("../logs");
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
