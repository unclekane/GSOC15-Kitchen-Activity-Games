#ifndef MAIN_WINDOW_QT
#define MAIN_WINDOW_QT

#include <gazebo/transport/transport.hh>
#include <QWidget>
#include <QThread>
#include <QByteArray>


class QPushButton;
class QCheckBox;
class QProcess;
class QTextEdit;


namespace gazebo
{
    class GUIWindow;

    class GAZEBO_VISIBLE GUIComClient : public QThread
    {
        Q_OBJECT

        private: GUIWindow *parent;

        public : GUIComClient(GUIWindow *p_parent) : parent(p_parent) {}
        private: void run();
    };


    class GAZEBO_VISIBLE GUIWindow : public QWidget
    {
      Q_OBJECT

      friend class GUIComClient;

      private: int         argc;
      private: char      **argv;
      private: QStringList args;
      private: QProcess *server_process;
      private: std::list<QProcess*> child_processes;
      private: QTextEdit *processOutputs;

      private: bool isSimulationPaused;
      private: bool isLoggingPaused;


      private: QPushButton *loggingButton;
      private: QPushButton *openLogButton;
      private: QPushButton *openWorldButton;
      private: QPushButton *openClientButton;
      private: QCheckBox   *pauseButton;
      private: QCheckBox   *startClientAuto;
      private: QCheckBox   *loadServerPlugin;
      private: QCheckBox   *loadClientPlugin;
      private: QCheckBox   *verboseOutput;

      private: transport::NodePtr      node;
      private: transport::PublisherPtr worldCntPub;
      private: transport::PublisherPtr serverCntPub;
      private: transport::PublisherPtr logCntPub;



      public: GUIWindow(int _argc, char **_argv);
      public: virtual ~GUIWindow();

      private: void startServer();
      private: void stopServer();
      private: void removeWorldOrLogFromArgs();
      private: void readProcessOutput(const char *p_process, const char *p_logLevel, QByteArray p_message);


      protected slots: void OnPauseButtonClick();
      protected slots: void OnOpenWorldClick();
      protected slots: void OnOpenLogButtonClick();
      protected slots: void OnLogButtonClick();
      protected slots: void OnOpenClientClick();
      protected slots: void OnLoadServerPluginClick();
      protected slots: void OnLoadClientPluginClick();

      protected slots: void OnVerboseClick();

      protected slots: void OnReadServerStdOutput();
      protected slots: void OnReadServerErrOutput();

      protected slots: void OnReadClientStdOutput();
      protected slots: void OnReadClientErrOutput();
    };
}
#endif
