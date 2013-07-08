#include "MainWindow.h"
#include "GLWindow.h"
#include <QApplication>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *_parent): QMainWindow(_parent), m_ui(new Ui::MainWindow)
{

  // setup the user interface
  m_ui->setupUi(this);
  // set the window size
  this->resize(QSize(1024,720));
  // create our GLWindow
  m_gl = new GLWindow(this);
  // set the tite bar
  this->setWindowTitle(QString("Hair Simulator"));

  m_ui->gridLayout->addWidget(m_gl, 0, 0, 0, 1);

  connect(m_ui->m_levels,SIGNAL(valueChanged(int)),m_gl,SLOT(setLevels(int)));

  connect(m_ui->m_restLength,SIGNAL(valueChanged(double)),m_gl,SLOT(setRestLength(double)));
  connect(m_ui->m_stiffness,SIGNAL(valueChanged(double)),m_gl,SLOT(setStiffness(double)));
  connect(m_ui->m_friction,SIGNAL(valueChanged(double)),m_gl,SLOT(setFriction(double)));
  connect(m_ui->m_gravity,SIGNAL(valueChanged(double)),m_gl,SLOT(setGravity(double)));
  connect(m_ui->m_nRoots,SIGNAL(valueChanged(int)),m_gl,SLOT(setNRoots(int)));
  connect(m_ui->m_levels,SIGNAL(valueChanged(int)),m_gl,SLOT(setLevels(int)));
  connect(m_ui->m_update,SIGNAL(clicked()),m_gl,SLOT(update()));
  connect(m_ui->m_seeParticles,SIGNAL(toggled(bool)),m_gl,SLOT(seeParticles(bool)));
  connect(m_ui->m_wind,SIGNAL(valueChanged(double)),m_gl,SLOT(setWindMagnitude(double)));

   //m_gl->startSimTimer();

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

void MainWindow::resizeEvent ( QResizeEvent * _event )
{
  m_gl->resize(_event->size());
}


