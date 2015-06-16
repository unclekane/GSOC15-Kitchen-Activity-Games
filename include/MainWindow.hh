#ifndef MAIN_WINDOW_QT
#define MAIN_WINDOW_QT

#include <gazebo/transport/transport.hh>
#include <QWidget>

class QPushButton;
class QProcess;

namespace gazebo
{
    class GAZEBO_VISIBLE GUIWindow : public QWidget
    {
      Q_OBJECT

      public: GUIWindow(int _argc, char **_argv);
      public: virtual ~GUIWindow();

      private: int argc;
      private: char **argv;
      private: QProcess *server_process;

      private: bool isSimulationPaused;
      private: bool isLoggingPaused;
      private: QPushButton *pauseButton;
      private: QPushButton *loggingButton;
      private: QPushButton *startServerButton;
      private: QPushButton *openWorldButton;

      protected slots: void OnStartServerButtonClick();
      protected slots: void OnPauseButtonClick();
      protected slots: void OnOpenWorldClick();
      protected slots: void OnLogButtonClick();

      private: transport::NodePtr node;
      private: transport::PublisherPtr worldCntPub;
      private: transport::PublisherPtr serverCntPub;
      private: transport::PublisherPtr logCntPub;
    };
}
#endif
