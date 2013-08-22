#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file MainWindow.h
 * @brief This is the full window that holds the GUI and the GLWindow
 */

#include <QMainWindow>
#include "GLWindow.h"

namespace Ui
{
    class MainWindow;
}

/**
 * @class MainWindow
 * @brief This is the full window that holds the GUI and the GLWindow
 */
class MainWindow : public QMainWindow
{

Q_OBJECT

private:
    GLWindow *m_gl;
    Ui::MainWindow *m_ui;

private slots :
    /* Here we add our GLWindow extending the base functionality of our class*/
    void setTimerInterval(int _interval);
    void setStep(int _step);

protected :
    /*Overwritten methods from QObject*/
    void keyPressEvent(QKeyEvent *_event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
