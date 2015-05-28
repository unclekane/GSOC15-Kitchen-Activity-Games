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

      private: bool isPaused;
      private: QPushButton *pauseButton;

      protected slots: void OnPauseButtonClick();
      protected slots: void OnOpenWorldClick();

      private: transport::NodePtr node;
      private: transport::PublisherPtr worldCntPub;
      private: transport::PublisherPtr serverCntPub;
    };
}
#endif
