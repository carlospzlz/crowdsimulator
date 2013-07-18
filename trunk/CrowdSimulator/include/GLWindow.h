#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__


#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/SpotLight.h>
#include <ngl/Transformation.h>
#include <ngl/TransformStack.h>
#include <ngl/Obj.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
// it must be included after our stuff becuase GLEW needs to be first
#include <QResizeEvent>
#include <QEvent>
#include "CrowdEngine.h"

class GLWindow : public QGLWidget
{

// this must include this if you use Qt signals/slots
Q_OBJECT

private :
    static const float s_rotationIncrement;
    static const float s_translationIncrement;
    static const float s_zoomIncrement;
    static const int s_groundSize;
    static const int s_timerValue;
    int m_timer;
    bool m_simulating;
    bool m_rotate;
    bool m_translate;
    std::pair<int,int> m_previousMousePosition;
    ngl::Vec2 m_globalRotation;
    ngl::Vec3 m_globalTranslation;
    ngl::TransformStack m_transformStack;
    ngl::Transformation m_transformation;
    ngl::VAOPrimitives *m_primitives;
    ngl::ShaderLib *m_shader;
    ngl::Camera m_camera;
    ngl::Light m_light;
    ngl::Obj *m_dummy;
    ngl::Obj *m_dummy2;

    CrowdEngine m_crowdEngine;

    void loadMatricesToShader(ngl::TransformStack &_tx);
    void loadMVPToShader(ngl::TransformStack &_tx);
    void inline drawVector(ngl::Vec4 _vector);
    void inline drawRadius(int _radius);
    void inline drawStrength(float _strength, float _mass);
    void inline setStateColour(std::string _state);

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
    inline void toggleSimulating(){ m_simulating = !m_simulating; }

protected:

    void initializeGL();
    void resizeGL(const int _w,const int _h);
    void paintGL();

};

#endif
