#include <gazebo/msgs/msgs.hh>
#include <gazebo/util/util.hh>

#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QPlainTextEdit>


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

using namespace gazebo;


/////////////////////////////////////////////////
GUIWindow::GUIWindow(int p_argc, char **p_argv) : QWidget()
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    server_process = NULL;
    this->argc = p_argc;
    this->argv = p_argv;
    processOutputs = NULL;

    this->setStyleSheet("QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

    // Create the main layout
    QHBoxLayout *mainLayout  = new QHBoxLayout;
    QFrame      *mainFrame   = new QFrame();
    QVBoxLayout *frameLayout = new QVBoxLayout();


    openWorldButton = new QPushButton("Start World", this);
    connect(openWorldButton, SIGNAL(clicked()), this, SLOT(OnOpenWorldClick()));
    frameLayout->addWidget(openWorldButton);

    openLogButton = new QPushButton("Start Log", this);
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


    mainFrame->setLayout(frameLayout);
    mainLayout->addWidget(mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(mainLayout);


    isSimulationPaused = false;
    isLoggingPaused    = true;

    for (int i = 1; i < this->argc; ++i)
    {
        args.push_back(this->argv[i]);
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

        delete processOutputs;
    }
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

        args.append("-s");
        args.append(file);
    }
    else
    {
        for(int i = 0; i < args.size(); i++)
        {
            if( args.at(i).contains("-s") )
            {
                args.removeAt(i);
                args.removeAt(i + 1);
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

        if(file.isEmpty())
        {
            loadClientPlugin->setChecked(false);
            return;
        }

        args.append("-g " + file);
    }
    else
    {
        for(int i = 0; i < args.size(); i++)
        {
            if( args.at(i).contains("-g") )
            {
                args.removeAt(i);
                args.removeAt(i + 1);
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
            worldMsg.set_pause(false);
            isSimulationPaused = false;
        }
        else
        {
            worldMsg.set_pause(true);
            isSimulationPaused = true;
        }

        this->worldCntPub->Publish(worldMsg);
    }
    else
    {
        if(pauseButton->checkState())
        {
            args.append("-u");
        }
        else
        {
            if( int indx = args.indexOf("-u") > 0 )
            {
                args[indx] = "";
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
    for(int i = 0; i < args.size(); i++)
    {
        if( args.at(i).contains(".sdf") )
        {
            args.removeAt(i);
            break;
        }

        if( args.at(i).contains("-p") )
        {
            args.removeAt(i);
            args.removeAt(i + 1);
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

        args.push_back("-p");
        args.push_back(file);
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

        args.push_back(file);
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
    server_process->start("./gzserver", args);

    if( verboseOutput->isChecked() )
    {
        connect(server_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadServerStdOutput()));
        connect(server_process, SIGNAL(readyReadStandardError()),  this, SLOT(OnReadServerErrOutput()));
    }

    pauseButton->setDisabled(false);
    loggingButton->setDisabled(false);
    openClientButton->setDisabled(false);

    GUIComClient *clientThread = new GUIComClient(this);
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

    server_process->terminate();
    server_process->waitForFinished(-1);
    server_process->kill();
    delete server_process;
    server_process = NULL;


    for( std::list<QProcess*>::iterator processItr = child_processes.begin(); processItr != child_processes.end(); ++processItr)
    {
        (*processItr)->terminate();
        (*processItr)->kill();
        (*processItr)->waitForFinished(-1);
        delete (*processItr);
    }
    child_processes.clear();


    pauseButton->setDisabled(true);
    loggingButton->setDisabled(true);
    openClientButton->setDisabled(true);

    openWorldButton->setText("Start World");
    openLogButton->setText("Start Log");
}


/////////////////////////////////////////////////
void GUIWindow::OnOpenClientClick()
{
    QProcess *client_process = new QProcess(this);
    client_process->start("gzclient");
    child_processes.push_back(client_process);

    if( verboseOutput->isChecked() )
    {
        connect(client_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadClientStdOutput()));
        connect(client_process, SIGNAL(readyReadStandardError()),  this, SLOT(OnReadClientErrOutput()));
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

    // Create a node for transportation
    parent->node = transport::NodePtr(new transport::Node());
    parent->node->Init();
    parent->worldCntPub  = parent->node->Advertise<msgs::WorldControl>("~/world_control");
    parent->serverCntPub = parent->node->Advertise<msgs::ServerControl>("/gazebo/server/control");
    parent->logCntPub    = parent->node->Advertise<msgs::LogControl>("~/log/control");


    gazebo::msgs::LogControl logCntlr_path, logCntlr_encode;
    logCntlr_path.set_base_path(LOGS_FOLDER);
    logCntlr_encode.set_encoding("txt");
    parent->logCntPub->Publish(logCntlr_path);
    parent->logCntPub->Publish(logCntlr_encode);

    while (true)
      gazebo::common::Time::MSleep(10);

    gazebo::shutdown();
}


void GUIWindow::OnVerboseClick()
{
    if( verboseOutput->isChecked() )
    {
        args.append("--verbose");

        if( processOutputs == NULL )
        {
            processOutputs = new QPlainTextEdit();
            processOutputs->show();
        }
    }
    else
    {
        if( int indx = args.indexOf("--verbose") > 0 )
        {
            args[indx] = "";
        }

        if( processOutputs != NULL )
        {
            delete processOutputs;
            processOutputs = NULL;
        }
    }
}

/////////////////////////////////////////////////
void GUIWindow::readProcessOutput( const char *p_process, const char *p_logLevel, QByteArray p_message )
{
    QString message;
    message.append(p_process);
    message.append(" ");
    message.append(p_logLevel);
    message.append(" : ");

    QString tmp = QObject::trUtf8(p_message);
    tmp.replace("\033[32m","");
    tmp.replace("\033[33m","");
    tmp.replace("\033[0m","");
    tmp.replace("\033]0;~","");
    tmp.replace("\033[1m","");
    tmp.replace("\033[2m","");
    tmp.replace("\033[3m","");
    tmp.replace("\033[4m","");
    tmp.replace("\033[5m","");
    tmp.replace("\033[6m","");
    tmp.replace("\033[7m","");
    tmp.replace("\007","");

    message.append(tmp);

    processOutputs->appendPlainText(message);
}


/////////////////////////////////////////////////
void GUIWindow::OnReadServerStdOutput()
{
    readProcessOutput( "server", "vebose", server_process->readAllStandardOutput() );
}


/////////////////////////////////////////////////
void GUIWindow::OnReadServerErrOutput()
{
    readProcessOutput( "Server", "error", server_process->readAllStandardError() );
}


/////////////////////////////////////////////////
void GUIWindow::OnReadClientStdOutput()
{
    readProcessOutput( "Client", "verbose", server_process->readAllStandardOutput() );
}


/////////////////////////////////////////////////
void GUIWindow::OnReadClientErrOutput()
{
    readProcessOutput( "Client", "error", server_process->readAllStandardError() );
}
