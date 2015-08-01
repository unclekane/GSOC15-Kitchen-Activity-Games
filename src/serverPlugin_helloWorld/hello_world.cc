#include <gazebo/gazebo.hh>

namespace gazebo
{
  class SystemPl : public SystemPlugin
  {
    public: SystemPl() : SystemPlugin()
            {
              printf("Hello World!\n");
            }

    public: virtual void Load(int _argc = 0, char **_argv = NULL)
      {
           printf("Hello World!\n");

           for (unsigned int i = 0; i < _argc; ++i){
               printf( _argv[i] );
               printf("\n");
           }
      }
  };

  GZ_REGISTER_SYSTEM_PLUGIN(SystemPl)
}
