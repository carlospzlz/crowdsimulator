#include "MainWindow.h"
#include "GLWindow.h"
#include <QApplication>
//#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *_parent): QMainWindow(_parent)//, m_ui(new Ui::MainWindow)
{

  // setup the user interface
  //m_ui->setupUi(this);
  // set the window size
  this->resize(QSize(1024,720));
  // create our GLWindow
  m_gl = new GLWindow(this);

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


