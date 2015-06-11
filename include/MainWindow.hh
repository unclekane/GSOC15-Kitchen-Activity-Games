#ifndef MAIN_WINDOW_QT
#define MAIN_WINDOW_QT

#include <gazebo/transport/transport.hh>
#include <QWidget>

class QPushButton;

namespace gazebo
{
    class GAZEBO_VISIBLE GUIWindow : public QWidget
    {
      Q_OBJECT

      public: GUIWindow();
      public: virtual ~GUIWindow();

      private: bool isSimulationPaused;
      private: bool isLoggingPaused;
      private: QPushButton *pauseButton;
      private: QPushButton *loggingButton;

      protected slots: void OnPauseButtonClick();
      protected slots: void OnOpenWorldClick();
      protected slots: void OnLogButtonClick();

      private: transport::NodePtr node;
      private: transport::PublisherPtr worldCntPub;
      private: transport::PublisherPtr serverCntPub;
    };
}
#endif
