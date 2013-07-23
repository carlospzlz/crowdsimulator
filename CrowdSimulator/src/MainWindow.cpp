#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *_parent): QMainWindow(_parent), m_ui(new Ui::MainWindow)
{
    // setup the user interface
    m_ui->setupUi(this);
    // set the window size
    // this->resize(QSize(1024,720));
    // create our GLWindow
    m_gl = new GLWindow(this);

    m_ui->s_windowLayout->addWidget(m_gl,0,0,6,3);

    // WIRE UP THE COMPONENST OF THE GUI TO THE FUNCTIONALITIES

    // terrain

    // crowd
    connect(m_ui->s_loadBrainButton,SIGNAL(clicked()),m_gl,SLOT(loadBrains()));
    connect(m_ui->s_loadCrowdButton,SIGNAL(clicked()),m_gl,SLOT(loadCrowds()));

    // cell partition
    connect(m_ui->s_drawCells,SIGNAL(toggled(bool)),m_gl,SLOT(setDrawCells(bool)));
    connect(m_ui->s_cellSizeSpinbox,SIGNAL(valueChanged(int)),m_gl,SLOT(rearrangeCellPartition(int)));

    // collisions
    connect(m_ui->s_drawBoundingSphere,SIGNAL(toggled(bool)),m_gl,SLOT(setDrawCollisionRadius(bool)));
    connect(m_ui->s_radiusMass,SIGNAL(valueChanged(double)),m_gl,SLOT(scaleCollisionRadius(double)));
    // simulation
    connect(m_ui->s_drawVelocityVector,SIGNAL(toggled(bool)),m_gl,SLOT(setDrawVelocityVector(bool)));
    connect(m_ui->s_drawVisionRadius,SIGNAL(toggled(bool)),m_gl,SLOT(setDrawVisionRadius(bool)));
    connect(m_ui->s_drawStrength,SIGNAL(toggled(bool)),m_gl,SLOT(setDrawStrength(bool)));
    connect(m_ui->s_shaderComboBox,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(setShader(int)));
    connect(m_ui->s_dummyComboBox,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(setDummyIndex(int)));
    connect(m_ui->s_simulateButton,SIGNAL(clicked(bool)),m_gl,SLOT(toggleSimulation(bool)));
    connect(m_ui->s_restartButton,SIGNAL(clicked()),m_gl,SLOT(restart()));
    connect(m_ui->s_clearButton,SIGNAL(clicked()),m_gl,SLOT(clear()));

    // sliders
    connect(m_ui->s_timerSlider,SIGNAL(valueChanged(int)),this,SLOT(setTimerInterval(int)));
    connect(m_ui->s_stepSlider,SIGNAL(valueChanged(int)),this,SLOT(setStep(int)));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  default : break;
  }
  // finally update the GLWindow and re-draw
  m_gl->updateGL();
}

void MainWindow::setTimerInterval(int _interval)
{
    m_gl->setTimerInterval(_interval);
    m_ui->s_timerLcd->display(_interval);
}

void MainWindow::setStep(int _step)
{
    m_gl->setStep(_step/100.0);
    m_ui->s_stepLcd->display(_step/100.0);
}
