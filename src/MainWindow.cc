#include <gazebo/msgs/msgs.hh>
#include <gazebo/util/util.hh>

#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>

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


using namespace gazebo;


/////////////////////////////////////////////////
GUIWindow::GUIWindow(int p_argc, char **p_argv) : QWidget()
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    server_process = NULL;
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

    // Create a push button, and connect it to the OnButton function
    pauseButton = new QCheckBox("Paused");
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClick()));
    frameLayout->addWidget(pauseButton);


    // Create a push button, and connect it to the OnButton function
    startClientAuto = new QCheckBox("Auto Client start");
    startClientAuto->setChecked(true);
    frameLayout->addWidget(startClientAuto);



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
