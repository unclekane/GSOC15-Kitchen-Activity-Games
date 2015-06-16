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


    // Create a push button, and connect it to the OnButton function
    pauseButton = new QPushButton("Stop Simulation");
    pauseButton->setDisabled(true);
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClick()));
    frameLayout->addWidget(pauseButton);


    openWorldButton = new QPushButton("Open World", this);
    openWorldButton->setDisabled(true);
    connect(openWorldButton, SIGNAL(clicked()), this, SLOT(OnOpenWorldClick()));
    frameLayout->addWidget(openWorldButton);


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

    // Create a node for transportation
    this->node = transport::NodePtr(new transport::Node());
    this->node->Init();
    this->worldCntPub  = this->node->Advertise<msgs::WorldControl>("~/world_control");
    this->serverCntPub = this->node->Advertise<msgs::ServerControl>("/gazebo/server/control");
    this->logCntPub    = this->node->Advertise<msgs::LogControl>("~/log/control");


    gazebo::msgs::LogControl logCntlr_path, logCntlr_encode;
    logCntlr_path.set_base_path("logs");
    logCntlr_encode.set_encoding("txt");
    this->logCntPub->Publish(logCntlr_path);
    this->logCntPub->Publish(logCntlr_encode);
}

/////////////////////////////////////////////////
GUIWindow::~GUIWindow()
{
}

/////////////////////////////////////////////////
void GUIWindow::OnStartServerButtonClick()
{
    // start server
    QStringList args;
    for (int i = 1; i < this->argc; ++i)
    {
        args.push_back(this->argv[i]);
    }

    if(server_process != NULL)
    {
        server_process->kill();
        delete server_process;
    }

    server_process = new QProcess(this);
    server_process->start("./ServerMain", args);


    // start minimal communication cClient
    gzLogInit("client-", "client.log");

    if( gazebo::setupClient(this->argc, this->argv) )
    {
        startServerButton->setDisabled(true);
        pauseButton->setDisabled(false);
        openWorldButton->setDisabled(false);
        loggingButton->setDisabled(false);
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


void GUIWindow::OnOpenWorldClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open World"), "/home");
    gazebo::msgs::ServerControl serverCntrl;
    serverCntrl.set_open_filename(file.toStdString());
    this->serverCntPub->Publish(serverCntrl);
}
