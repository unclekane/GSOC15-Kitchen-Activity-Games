#ifndef MAIN_WINDOW_QT
#define MAIN_WINDOW_QT

#include <gazebo/transport/transport.hh>
#include <QWidget>
#include <QThread>
#include <QByteArray>
#include <QSettings>
#include "PlayList.hh"


class QPushButton;
class QCheckBox;
class QProcess;
class QListWidget;
class QLineEdit;


namespace gazebo
{
    class GUIWindow;

    /**
     * A minimal communication client, which will be used to communicate with the server.
     * It will receive world statistics and determine with them if the server is running.
     * The class is run by a QThread from GUIWindow.
     */
    class GAZEBO_VISIBLE GUIComClient : public QThread
    {
        Q_OBJECT

        private: GUIWindow *parent;
        public: bool DoRun;

        public : GUIComClient(GUIWindow *p_parent) : parent(p_parent) {}

        /**
         * The world statistics are received by this function. It is determining if log files are processed and the server is in idle mode again.
         */
        private: void aliveMsgHandler(ConstWorldStatisticsPtr &p_msg);

        /**
         * Runs a gazebo client with topic subscriptions, such as the world statistics.
         */
        private: void run();

        private: signals: void nextLog();

    };

    /**
     * Struct to represent an argument. It saves key value pair.
     */
    struct Argument
    {
        QString   Text;
        QVariant  Value;

        bool isIncremented;
        bool isServerArg;
        bool isClientArg;

        /**
         * Constructor
         * Creates an Argument wit key value pair and membership
         * @param QString p_text Key of the argument
         * @param QString p_value Value of the argument, is converted to a number if the p_increment is true
         * @param bool p_increment if the value should be incremented, will fail if is not a number.
         * @param bool p_serverArg if the argument is a server argument
         * @param bool p_clientArg if the argument is a client argument
         *
         */
        Argument( QString p_text, QString p_value, bool p_increment, bool p_serverArg, bool p_clientArg )
          : Text(p_text),
            isIncremented(p_increment),
            isServerArg(p_serverArg),
            isClientArg(p_clientArg)
        {
            if( !p_value.isEmpty() )
            {
                if( isIncremented )
                {
                    Value = p_value.toInt();
                }
                else
                {
                    Value = p_value;
                }
            }
        }
    };

    /**
     * Main class of the project.
     * It is running GUIComClient in a QThread for server communication.
     * And using a GUI to control the application behavior.
     * The input elements enables starting, stoping, pausing, ect. the server.
     * The instance of this class is created from MinimalClientMain.cc.
     */
    class GAZEBO_VISIBLE GUIWindow : public QWidget
    {
      Q_OBJECT

      friend class GUIComClient;

      /**
       * The defined arguments in the arguments window are saved to a configuration file on exit.
       */
      private: QSettings settings;

      private: int     argc;
      private: char  **argv;

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
      private: QCheckBox   *customArgs;
      private: QCheckBox   *loadClientPlugin;
      private: QCheckBox   *verboseOutput;
      private: QCheckBox   *showPlaylist;


      private: QWidget     *argumentsWindow;
      private: QListWidget *argumentsListView;
      private: QLineEdit   *argText;
      private: QLineEdit   *valueText;
      private: QCheckBox   *incrementValue;
      private: QCheckBox   *serverArg;
      private: QCheckBox   *clientArg;


      private: QWidget     *playListWindow;
      private: PlayList    *playlistWidget;

      private: transport::NodePtr       node;
      private: transport::PublisherPtr  worldCntPub;
      private: transport::PublisherPtr  serverCntPub;
      private: transport::PublisherPtr  logCntPub;
      private: transport::SubscriberPtr aliveSubscrb;


      /**
       * Constructor
       * Creates all input elements, windows and defines varaibles.
       * @param int _argc the argument count of the application
       * @param char** _argv the char array with the arguments
       */
      public: GUIWindow(int _argc, char **_argv);

      /**
       * Destructor
       * Closes all windows on termination.
       */
      public: virtual ~GUIWindow();

      /**
       * Starting the server and stopping previouse server instances, with stopServer() if available.
       */
      private: void startServer();

      /**
       * Stoping the Server and all connected client instances.
       */
      private: void stopServer();

      /**
       * Removing world and log file arguments from server argument list.
       */
      private: void removeWorldOrLogFromArgs();

      /**
       * Plays next file in play list or stops the play list if the end is reached.
       */
      private: void playNextLog();

      /**
       * Adds files from string to the play list recursivly. All subfolders will be searched for log files.
       * @param QString with the path to the file or directory in following format: /home/ect/.../file.world
       */
      private: void addFileOrDirectoryToPlayList( QString &p_dir );

      protected slots: void OnPauseButtonClick();
      protected slots: void OnOpenWorldClick();
      protected slots: void OnOpenLogButtonClick();
      protected slots: void OnLogButtonClick();
      protected slots: void OnOpenClientClick();
      protected slots: void OnLoadServerPluginClick();
      protected slots: void OnLoadClientPluginClick();
      protected slots: void OnServerPluginsArgsClick();

      protected slots: void OnVerboseClick();

      protected slots: void OnShowPlaylist();
      protected slots: void OnAddToPlayBtnClick();
      protected slots: void OnRemoveFromPlayBtnClick();
      protected slots: void OnPlayBtnClick();
      protected slots: void OnPlayBtnFolderClick();


      protected slots: void OnSaveArg();
      protected slots: void OnRemoveArg();
      protected slots: void OnArgClicked(QListWidgetItem *);

      /**
       * Delegate for next log file. Is used from external class to determine that the current log file has been finished.
       */
      public slots: void NextLogFile();
    };
}
#endif
