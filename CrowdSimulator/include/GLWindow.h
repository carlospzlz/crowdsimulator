#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__


#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/SpotLight.h>
#include <ngl/TransformStack.h>
#include <ngl/Obj.h>
#include <ngl/NCCAPointBake.h>
// it must be included after our stuff becuase GLEW needs to be first
#include <QResizeEvent>
#include <QEvent>

class GLWindow : public QGLWidget
{

// this must include this if you use Qt signals/slots
Q_OBJECT

private :

    int m_spinXFace;
    int m_spinYFace;
    bool m_rotate;
    bool m_translate;
    int m_origX;
    int m_origY;
    int m_origXPos;
    int m_origYPos;
    bool m_active;

    void mouseMoveEvent(QMouseEvent * _event);
    void mousePressEvent(QMouseEvent *_event);
    void mouseReleaseEvent(QMouseEvent *_event);
    void timerEvent(QTimerEvent *_event);
    void wheelEvent(QWheelEvent *_event);

public :
    /**
     * @brief Constructor for GLWindow
     * @param [in] parent the parent window to create the GL context in
     */
    GLWindow(QWidget *_parent);
    ~GLWindow();
    inline void toggleActive(){m_active ^=true;}

protected:

    void initializeGL();
    void resizeGL(const int _w,const int _h);
    void paintGL();

};

#endif
