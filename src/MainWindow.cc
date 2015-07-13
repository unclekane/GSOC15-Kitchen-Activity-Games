#include <gazebo/msgs/msgs.hh>
#include <gazebo/util/util.hh>

#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QListWidget>


#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "gazebo/common/Console.hh"
#include "gazebo/Server.hh"
#include "gazebo/gui/GuiIface.hh"

#include "MainWindow.hh"

#define WORLDS_FOLDER "../worlds"
#define LOGS_FOLDER "../logs"
#define PLUGINS_SERVER_FOLDER "../plugins/server"
#define PLUGINS_CLIENT_FOLDER "../plugins/client"
#define WAIT_FOR_SERVER_SHUTDOWN_IN_MS 1000
#define WAIT_FOR_CLIENT_SHUTDOWN_IN_MS 5000
#define UNSAFE_SERVER_SHUTDOWN false

using namespace gazebo;


/////////////////////////////////////////////////
GUIWindow::GUIWindow(int p_argc, char **p_argv) : QWidget()
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    server_process = NULL;
    clientThread   = NULL;
    this->argc = p_argc;
    this->argv = p_argv;

    this->setStyleSheet("QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

    // Create the main layout
    QHBoxLayout *mainLayout  = new QHBoxLayout;
    QFrame      *mainFrame   = new QFrame();
    QVBoxLayout *frameLayout = new QVBoxLayout();


    openWorldButton = new QPushButton("Start World", this);
    connect(openWorldButton, SIGNAL(clicked()), this, SLOT(OnOpenWorldClick()));
    frameLayout->addWidget(openWorldButton);

    openLogButton = new QPushButton("Load Log", this);
    connect(openLogButton, SIGNAL(clicked()), this, SLOT(OnOpenLogButtonClick()));
    frameLayout->addWidget(openLogButton);


    loggingButton = new QPushButton("Start Logging");
    loggingButton->setDisabled(true);
    connect(loggingButton, SIGNAL(clicked()), this, SLOT(OnLogButtonClick()));
    frameLayout->addWidget(loggingButton);

    openClientButton = new QPushButton("Start Client");
    openClientButton->setDisabled(true);
    connect(openClientButton, SIGNAL(clicked()), this, SLOT(OnOpenClientClick()));
    frameLayout->addWidget(openClientButton);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    frameLayout->addWidget(line);


    pauseButton = new QCheckBox("Paused");
    pauseButton->setStyleSheet("color:#ffffff;");
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClick()));
    frameLayout->addWidget(pauseButton);


    startClientAuto = new QCheckBox("Auto Client start");
    startClientAuto->setChecked(true);
    startClientAuto->setStyleSheet("color:#ffffff;");
    frameLayout->addWidget(startClientAuto);

    showPlaylist = new QCheckBox("Playlist");
    showPlaylist->setStyleSheet("color:#ffffff;");
    connect(showPlaylist, SIGNAL(clicked()), this, SLOT(OnShowPlaylist()));
    frameLayout->addWidget(showPlaylist);


    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    frameLayout->addWidget(line2);


    loadServerPlugin = new QCheckBox("Load Server Plugin");
    loadServerPlugin->setStyleSheet("color:#ffffff;");
    connect(loadServerPlugin, SIGNAL(clicked()), this, SLOT(OnLoadServerPluginClick()));
    frameLayout->addWidget(loadServerPlugin);

    loadClientPlugin = new QCheckBox("Load Client Plugin");
    loadClientPlugin->setStyleSheet("color:#ffffff;");
    connect(loadClientPlugin, SIGNAL(clicked()), this, SLOT(OnLoadClientPluginClick()));
    frameLayout->addWidget(loadClientPlugin);


    QFrame* line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    frameLayout->addWidget(line3);

    verboseOutput = new QCheckBox("Verbose");
    verboseOutput->setStyleSheet("color:#ffffff;");
    connect(verboseOutput, SIGNAL(clicked()), this, SLOT(OnVerboseClick()));
    frameLayout->addWidget(verboseOutput);


    // PLAYLIST WINDOW

    playListWindow = new QWidget();
    playListWindow->setWindowTitle("Playlist");

    playlistWidget = new PlayList(playListWindow);
    playlistWidget->setGeometry(0, 0, 200, 300);



    QPushButton *addToPlayListBtn = new QPushButton(playListWindow);
    addToPlayListBtn->setText("+");
    addToPlayListBtn->setGeometry(0, 310, 20, 30);
    connect(addToPlayListBtn, SIGNAL(clicked()), this, SLOT(OnAddToPlayBtnClick()));

    QPushButton *removeFromPlayListBtn = new QPushButton(playListWindow);
    removeFromPlayListBtn->setText("-");
    removeFromPlayListBtn->setGeometry(30, 310, 20, 30);
    connect(removeFromPlayListBtn, SIGNAL(clicked()), this, SLOT(OnRemoveFromPlayBtnClick()));


    QPushButton *playBtn = new QPushButton(playListWindow);
    playBtn->setText("Play");
    playBtn->setGeometry(60, 310, 140, 30);
    connect(playBtn, SIGNAL(clicked()), this, SLOT(OnPlayBtnClick()));

    // END PLAYLIST WINDOW


    mainFrame->setLayout(frameLayout);
    mainLayout->addWidget(mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(mainLayout);


    isSimulationPaused = false;
    isLoggingPaused    = true;

    for (int i = 1; i < this->argc; ++i)
    {
        server_args.push_back(this->argv[i]);
        client_args.push_back(this->argv[i]);
    }
}


/////////////////////////////////////////////////
GUIWindow::~GUIWindow()
{
    if(server_process != NULL)
    {
        gazebo::msgs::LogControl logCntrl;
        logCntrl.set_stop(true);
        isLoggingPaused = true;
        this->logCntPub->Publish(logCntrl);

        stopServer();
    }

    playListWindow->close();
    delete playListWindow;
}


/////////////////////////////////////////////////
void GUIWindow::OnLoadServerPluginClick()
{
    if(loadServerPlugin->checkState())
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Server Plugin"), PLUGINS_SERVER_FOLDER);

        if(file.isEmpty())
        {
            loadServerPlugin->setChecked(false);
            return;
        }

        server_args.append("-s");
        server_args.append(file);
    }
    else
    {
        for(int i = 0; i < server_args.size(); i++)
        {
            if( server_args.at(i).contains("-s") )
            {
                server_args.removeAt(i);
                server_args.removeAt(i+1);
                break;
            }
        }
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnLoadClientPluginClick()
{
    if(loadClientPlugin->checkState())
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Client Plugin"), PLUGINS_CLIENT_FOLDER);

        if( file.isEmpty()
         && !file.contains(".so") )
        {
            loadClientPlugin->setChecked(false);
            return;
        }

        client_args.append("-g");
        client_args.append(file);
    }
    else
    {
        for(int i = 0; i < client_args.size(); i++)
        {
            if( client_args.at(i).contains("-g") )
            {
                client_args.removeAt(i);
                client_args.removeAt(i+1);
                break;
            }
        }
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnPauseButtonClick()
{
    if(server_process != NULL)
    {
        msgs::WorldControl worldMsg;

        if(pauseButton->checkState())
        {
            worldMsg.set_pause(true);
            isSimulationPaused = false;
        }
        else
        {
            worldMsg.set_pause(false);
            isSimulationPaused = true;
        }

        this->worldCntPub->Publish(worldMsg);
    }
    else
    {
        if(pauseButton->checkState())
        {
            server_args.append("-u");
            isSimulationPaused = true;
        }
        else
        {
            if( int indx = server_args.indexOf("-u") > 0 )
            {
                server_args[indx] = "";
                isSimulationPaused = false;
            }
        }
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnLogButtonClick()
{
    gazebo::msgs::LogControl logCntrl;

    if(isLoggingPaused)
    {
        logCntrl.set_start(true);
        isLoggingPaused = false;
        loggingButton->setText("Stop Logging");
    }
    else
    {
        logCntrl.set_stop(true);
        isLoggingPaused = true;
        loggingButton->setText("Start Logging");
    }

    this->logCntPub->Publish(logCntrl);
}


/////////////////////////////////////////////////
void GUIWindow::removeWorldOrLogFromArgs()
{
    for(int i = 0; i < server_args.size(); i++)
    {
        if( server_args.at(i).contains(".sdf") )
        {
            server_args.removeAt(i);
            break;
        }

        if( server_args.at(i).contains("-p") )
        {
            server_args.removeAt(i);
            server_args.removeAt(i + 1);
            break;
        }
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnOpenLogButtonClick()
{
    if(server_process == NULL)
    {
        removeWorldOrLogFromArgs();

        QString file = QFileDialog::getOpenFileName(this, tr("Open Log"), LOGS_FOLDER);

        if(file.isEmpty())
            return;

        server_args.push_back("-p");
        server_args.push_back(file);
        startServer();
    }
    else
    {
        stopServer();
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnOpenWorldClick()
{
    if(server_process == NULL)
    {
        removeWorldOrLogFromArgs();

        QString file = QFileDialog::getOpenFileName(this, tr("Open World"), WORLDS_FOLDER);

        if(file.isEmpty())
            return;

        server_args.push_back(file);
        startServer();
    }
    else
    {
        stopServer();
    }
}


/////////////////////////////////////////////////
void GUIWindow::startServer()
{
    if(server_process != NULL)
        stopServer();

    server_process = new QProcess(this);
    server_process->start("./gzserver", server_args);

    if( verboseOutput->isChecked() )
    {
        connect(server_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadServerStdOutput()));
        connect(server_process, SIGNAL(readyReadStandardError()),  this, SLOT(OnReadServerErrOutput()));
    }

    loggingButton->setDisabled(false);
    openClientButton->setDisabled(false);


    clientThread = new GUIComClient(this);
    clientThread->start();

    openWorldButton->setText("Stop Server");
    openLogButton->setText("Stop Server");

    if(startClientAuto->checkState())
        OnOpenClientClick();
}


void GUIWindow::stopServer()
{
    gazebo::msgs::ServerControl serverCntrl;
    serverCntrl.set_stop(true);
    this->serverCntPub->Publish(serverCntrl);

    server_process->waitForFinished(WAIT_FOR_SERVER_SHUTDOWN_IN_MS);


    if( UNSAFE_SERVER_SHUTDOWN )
    {
        server_process->terminate();
        server_process->waitForFinished(WAIT_FOR_SERVER_SHUTDOWN_IN_MS);
        server_process->kill();
        // kill the server if it is not responding and proceed
    }
    else
    {
        server_process->terminate();
        server_process->waitForFinished(-1);
        // wait until the server is shut down
    }


    delete server_process;
    server_process = NULL;

   // clientThread->DoRun = false;


    for( std::list<QProcess*>::iterator processItr = child_processes.begin(); processItr != child_processes.end(); ++processItr)
    {
        (*processItr)->terminate();
        (*processItr)->waitForFinished(WAIT_FOR_CLIENT_SHUTDOWN_IN_MS);
        (*processItr)->kill();
        delete (*processItr);
    }
    child_processes.clear();


    loggingButton->setDisabled(true);
    openClientButton->setDisabled(true);

    openWorldButton->setText("Start World");
    openLogButton->setText("Load Log");
}


/////////////////////////////////////////////////
void GUIWindow::OnOpenClientClick()
{
    QProcess *client_process = new QProcess(this);
    client_process->start("gzclient", client_args);
    child_processes.push_back(client_process);

    if( verboseOutput->isChecked() )
    {
        connect(client_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadClientStdOutput()));
        connect(client_process, SIGNAL(readyReadStandardError()),  this, SLOT(OnReadClientErrOutput()));
    }
}




/////////////////////////////////////////////////
void GUIComClient::aliveMsgHandler(ConstWorldStatisticsPtr &p_msg)
{
    std::cerr << p_msg->DebugString();

    if( p_msg->has_log_playback_stats()
     && p_msg->sim_time().nsec()  >= p_msg->log_playback_stats().end_time().nsec() )
    {
        emit nextLog();
    }
}


/////////////////////////////////////////////////
void GUIComClient::run()
{
    // start minimal communication cClient
    gzLogInit("client-", "comm_client.log");

    if( !gazebo::setupClient(parent->argc, parent->argv) )
    {
        std::cerr << "Client could not be started!";
    }

    connect(this, SIGNAL(nextLog()), parent, SLOT(NextLogFile()));

    // Create a node for transportation
    parent->node = transport::NodePtr(new transport::Node());
    parent->node->Init();
    parent->worldCntPub  = parent->node->Advertise<msgs::WorldControl>("~/world_control");
    parent->serverCntPub = parent->node->Advertise<msgs::ServerControl>("/gazebo/server/control");
    parent->logCntPub    = parent->node->Advertise<msgs::LogControl>("~/log/control");
    parent->aliveSubscrb = parent->node->Subscribe<gazebo::msgs::WorldStatistics, GUIComClient>("~/world_stats", &GUIComClient::aliveMsgHandler, this);

    gazebo::msgs::LogControl logCntlr_path, logCntlr_encode;
    logCntlr_path.set_base_path(LOGS_FOLDER);
    logCntlr_encode.set_encoding("txt");
    parent->logCntPub->Publish(logCntlr_path);
    parent->logCntPub->Publish(logCntlr_encode);

    DoRun = true;

    while(DoRun)
      gazebo::common::Time::MSleep(10);

    gazebo::shutdown();

    this->deleteLater();
}


void GUIWindow::OnVerboseClick()
{
    if( verboseOutput->isChecked() )
    {
        server_args.append("--verbose");
        client_args.append("--verbose");
    }
    else
    {
        if( int indx = server_args.indexOf("--verbose") > 0 )
        {
            server_args[indx] = "";
        }

        if( int indx = client_args.indexOf("--verbose") > 0 )
        {
            client_args[indx] = "";
        }
    }
}

/////////////////////////////////////////////////
void GUIWindow::readProcessOutput( const char *p_process, const char *p_logLevel, QByteArray p_message )
{
    if( p_message.size() > 0 )
    {
        QString message;
        message.append(p_process);
        message.append(" ");
        message.append(p_logLevel);
        message.append(" : ");

        message.append(p_message);

        std::cout << message.toStdString().c_str() << std::endl;
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnReadServerStdOutput()
{
    readProcessOutput( "Server", "vebose", server_process->readAllStandardOutput() );
}


/////////////////////////////////////////////////
void GUIWindow::OnReadServerErrOutput()
{
    readProcessOutput( "Server", "error", server_process->readAllStandardError() );
}


/////////////////////////////////////////////////
void GUIWindow::OnReadClientStdOutput()
{
    for( std::list<QProcess*>::iterator processItr = child_processes.begin(); processItr != child_processes.end(); ++processItr)
    {
        readProcessOutput( "Client", "verbose", (*processItr)->readAllStandardOutput() );
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnReadClientErrOutput()
{
    for( std::list<QProcess*>::iterator processItr = child_processes.begin(); processItr != child_processes.end(); ++processItr)
    {
        readProcessOutput( "Client", "error", (*processItr)->readAllStandardError() );
    }
}




void GUIWindow::OnShowPlaylist()
{
    if( showPlaylist->isChecked() )
    {
        playListWindow->show();
    }
    else
    {
        playListWindow->hide();
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnAddToPlayBtnClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Log File"), LOGS_FOLDER);

    if( file.isEmpty()
     && !file.contains(".log") )
        return;

    playlistWidget->addItem(file);
}


/////////////////////////////////////////////////
void GUIWindow::OnRemoveFromPlayBtnClick()
{
    qDeleteAll( playlistWidget->selectedItems() );
}


/////////////////////////////////////////////////
void GUIWindow::OnPlayBtnClick()
{
    playIndx = 0;

    if( playlistWidget->count() > 0 )
        playNextLog();
}


/////////////////////////////////////////////////
void GUIWindow::NextLogFile()
{
    playNextLog();
}


/////////////////////////////////////////////////
void GUIWindow::playNextLog()
{
    if( playlistWidget->count() > playIndx )
    {
        std::cerr << " play: " << playIndx << std::endl ;
        removeWorldOrLogFromArgs();

        playlistWidget->setCurrentRow(playIndx);
        QString file = playlistWidget->item(playIndx)->text();

        if(file.isEmpty())
            return;

        server_args.push_back("-p");
        server_args.push_back(file);
        startServer();

        playIndx++;
    }
    else if( playlistWidget->count() > 0 )
    {
        removeWorldOrLogFromArgs();
        stopServer();
        playIndx = 0;
        std::cerr << " end";
    }
}
