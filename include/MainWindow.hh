#ifndef MAIN_WINDOW_QT
#define MAIN_WINDOW_QT

#include <gazebo/transport/transport.hh>
#include <QWidget>
#include <QThread>
#include <QByteArray>
#include "PlayList.hh"


class QPushButton;
class QCheckBox;
class QProcess;
class QListWidget;


namespace gazebo
{
    class GUIWindow;

    class GAZEBO_VISIBLE GUIComClient : public QThread
    {
        Q_OBJECT

        private: GUIWindow *parent;
        public: bool DoRun;

        public : GUIComClient(GUIWindow *p_parent) : parent(p_parent) {}

        private: void aliveMsgHandler(ConstWorldStatisticsPtr &p_msg);
        private: void run();

        private: signals: void nextLog();

    };


    class GAZEBO_VISIBLE GUIWindow : public QWidget
    {
      Q_OBJECT

      friend class GUIComClient;

      private: int         argc;
      private: char      **argv;

      private: QStringList   server_args;
      private: QStringList   client_args;
      private: QProcess     *server_process;
      private: GUIComClient *clientThread;
      private: std::list<QProcess*> child_processes;

      private: bool isSimulationPaused;
      private: bool isLoggingPaused;

      private: int playIndx;


      private: QPushButton *loggingButton;
      private: QPushButton *openLogButton;
      private: QPushButton *openWorldButton;
      private: QPushButton *openClientButton;
      private: QCheckBox   *pauseButton;
      private: QCheckBox   *startClientAuto;
      private: QCheckBox   *loadServerPlugin;
      private: QCheckBox   *loadClientPlugin;
      private: QCheckBox   *verboseOutput;
      private: QCheckBox   *showPlaylist;


      private: QWidget     *playListWindow;
      private: PlayList    *playlistWidget;

      private: transport::NodePtr       node;
      private: transport::PublisherPtr  worldCntPub;
      private: transport::PublisherPtr  serverCntPub;
      private: transport::PublisherPtr  logCntPub;
      private: transport::SubscriberPtr aliveSubscrb;


      public: GUIWindow(int _argc, char **_argv);
      public: virtual ~GUIWindow();

      private: void startServer();
      private: void stopServer();
      private: void removeWorldOrLogFromArgs();
      private: void readProcessOutput(const char *p_process, const char *p_logLevel, QByteArray p_message);
      private: void aliveMsgHandler(ConstWorldStatisticsPtr &_msg);
      private: void playNextLog();
      private: void addFileOrDirectoryToPlayList( QString &p_dir );

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

      protected slots: void OnShowPlaylist();
      protected slots: void OnAddToPlayBtnClick();
      protected slots: void OnRemoveFromPlayBtnClick();
      protected slots: void OnPlayBtnClick();
      protected slots: void OnPlayBtnFolderClick();

      public slots: void NextLogFile();
    };
}
#endif
