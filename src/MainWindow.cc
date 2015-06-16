#include <gazebo/msgs/msgs.hh>
#include <gazebo/util/util.hh>

#include "gazebo/gui/qt.h"
#include "gazebo/gazebo.hh"

#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>

#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "gazebo/common/Console.hh"
#include "gazebo/Server.hh"
#include "gazebo/gui/GuiIface.hh"


#include "MainWindow.hh"

using namespace gazebo;


/////////////////////////////////////////////////
GUIWindow::GUIWindow(int p_argc, char **p_argv) : QWidget()
{
    server_process = NULL;
    this->argc = p_argc;
    this->argv = p_argv;

    this->setStyleSheet("QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

    // Create the main layout
    QHBoxLayout *mainLayout  = new QHBoxLayout;
    QFrame      *mainFrame   = new QFrame();
    QVBoxLayout *frameLayout = new QVBoxLayout();


    startServerButton = new QPushButton("Start Server");
    connect(startServerButton, SIGNAL(clicked()), this, SLOT(OnStartServerButtonClick()));
    frameLayout->addWidget(startServerButton);

    openWorldButton = new QPushButton("Open World and Start", this);
    connect(openWorldButton, SIGNAL(clicked()), this, SLOT(OnOpenWorldClick()));
    frameLayout->addWidget(openWorldButton);


    // Create a push button, and connect it to the OnButton function
    pauseButton = new QPushButton("Stop Simulation");
    pauseButton->setDisabled(true);
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClick()));
    frameLayout->addWidget(pauseButton);

    loggingButton = new QPushButton("Start Logging");
    loggingButton->setDisabled(true);
    connect(loggingButton, SIGNAL(clicked()), this, SLOT(OnLogButtonClick()));
    frameLayout->addWidget(loggingButton);


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
        stopServer();
}


/////////////////////////////////////////////////
void GUIWindow::OnStartServerButtonClick()
{
    if(server_process == NULL)
    {
        startServer();
        startServerButton->setText("Stop Server");
    }
    else
    {
        stopServer();
    }
}


/////////////////////////////////////////////////
void GUIWindow::OnPauseButtonClick()
{
    msgs::WorldControl worldMsg;

    if(isSimulationPaused)
    {
        worldMsg.set_pause(false);
        isSimulationPaused = false;
        pauseButton->setText("Stop Simulation");
    }
    else
    {
        worldMsg.set_pause(true);
        isSimulationPaused = true;
        pauseButton->setText("Start Simulation");
    }

    this->worldCntPub->Publish(worldMsg);
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
void GUIWindow::OnOpenWorldClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open World"), "/home");
    /* NOT IMPLEMENTED IN GAZEBO YET!!!
    gazebo::msgs::ServerControl serverCntrl;
    serverCntrl.set_open_filename(file.toStdString());
    this->serverCntPub->Publish(serverCntrl);
    */

    args.push_back(file);
    startServer();
}


/////////////////////////////////////////////////
void GUIWindow::startServer()
{
    if(server_process != NULL)
        stopServer();

    server_process = new QProcess(this);
    server_process->start("./gzserver", args);

    pauseButton->setDisabled(false);
    openWorldButton->setDisabled(false);
    loggingButton->setDisabled(false);

    GUIComClient *clientThread = new GUIComClient(this);
    clientThread->start();
}


void GUIWindow::stopServer()
{
    server_process->terminate();
    server_process->waitForFinished(1000);
    server_process->kill();
    delete server_process;
    server_process = NULL;

    pauseButton->setDisabled(true);
    openWorldButton->setDisabled(true);
    loggingButton->setDisabled(true);

    startServerButton->setText("Start Server");
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
    logCntlr_path.set_base_path("logs");
    logCntlr_encode.set_encoding("txt");
    parent->logCntPub->Publish(logCntlr_path);
    parent->logCntPub->Publish(logCntlr_encode);

    while (true)
      gazebo::common::Time::MSleep(10);

    gazebo::shutdown();
}
