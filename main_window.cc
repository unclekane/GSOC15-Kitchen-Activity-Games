#include <gazebo/msgs/msgs.hh>

#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFileDialog>

#include "main_window.hh"

using namespace gazebo;


/////////////////////////////////////////////////
GUIWindow::GUIWindow() : QWidget()
{
  this->setStyleSheet("QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the main layout
  QHBoxLayout *mainLayout  = new QHBoxLayout;
  QFrame      *mainFrame   = new QFrame();
  QVBoxLayout *frameLayout = new QVBoxLayout();

  // Create a push button, and connect it to the OnButton function
  pauseButton = new QPushButton("Stop Simulation");
  connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClick()));
  frameLayout->addWidget(pauseButton);

  QPushButton *openWorldButton = new QPushButton("Open World");
  connect(openWorldButton, SIGNAL(clicked()), this, SLOT(OnOpenWorldClick()));
  frameLayout->addWidget(openWorldButton);


  mainFrame->setLayout(frameLayout);
  mainLayout->addWidget(mainFrame);
  frameLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(mainLayout);


  isPaused = false;

  // Create a node for transportation
  this->node = transport::NodePtr(new transport::Node());
  this->node->Init();
  this->worldCntPub  = this->node->Advertise<msgs::WorldControl>("~/world");
  this->serverCntPub = this->node->Advertise<msgs::ServerControl>("/gazebo/server/control");
}

/////////////////////////////////////////////////
GUIWindow::~GUIWindow()
{
}

/////////////////////////////////////////////////
void GUIWindow::OnPauseButtonClick()
{
  msgs::WorldControl worldMsg;

  if(!isPaused)
  {
      worldMsg.pause();
      isPaused = true;
      pauseButton->setText("Start Simulation");
  }
  else
  {
      worldMsg.clear_pause();
      isPaused = false;
      pauseButton->setText("Stop Simulation");
  }

  this->worldCntPub->Publish(worldMsg);
}


void GUIWindow::OnOpenWorldClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open World"), "/home");
    gazebo::msgs::ServerControl serverCntrl;
    serverCntrl.set_open_filename(file.toStdString());
    this->serverCntPub->Publish(serverCntrl);
}
