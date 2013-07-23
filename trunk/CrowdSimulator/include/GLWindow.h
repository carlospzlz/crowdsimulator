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
#include <QTimer>
#include <QFileDialog>

#include "TXTParser.h"
#include "CrowdEngine.h"
#include "QuadraticGridCP.h"

enum shader{phong,colour};

class GLWindow : public QGLWidget
{

// this must include this if you use Qt signals/slots
Q_OBJECT

private :
    static const QString s_brainsPath;
    static const QString s_crowdsPath;
    static const float s_rotationIncrement;
    static const float s_translationIncrement;
    static const float s_zoomIncrement;
    static const int s_groundSize;
    static const int s_timerValue;
    QTimer *m_timer;
    //int m_timer;
    //bool m_simulating;
    bool m_rotate;
    bool m_translate;
    std::pair<int,int> m_previousMousePosition;
    ngl::Vec2 m_globalRotation;
    ngl::Vec3 m_globalTranslation;
    ngl::TransformStack m_transformStack;
    ngl::Transformation m_transformation;
    ngl::VAOPrimitives *m_primitives;
    ngl::ShaderLib *m_shader;
    int m_shaderIndex;
    ngl::Camera m_camera;
    ngl::Light m_light;
    std::vector<ngl::Obj*> m_dummies;
    int m_dummyIndex;
    ngl::VertexArrayObject *m_boidVAO;
    bool m_drawCollisionRadius;
    bool m_drawCells;
    bool m_drawVelocityVector;
    bool m_drawVisionRadius;
    bool m_drawStrength;

    Parser* m_parser;
    CrowdEngine m_crowdEngine;

    void buildBoidVAO();
    void loadMatricesToShader(ngl::TransformStack &_tx);
    void inline drawCollisionRadius(float _collisionRadius);
    void inline drawVector(ngl::Vec4 _vector);
    void inline drawRadius(float _radius);
    void inline drawStrength(float _strength, float _mass);
    void inline setStateColour(std::string _state);

    void mouseMoveEvent(QMouseEvent * _event);
    void mousePressEvent(QMouseEvent *_event);
    void mouseReleaseEvent(QMouseEvent *_event);
    void wheelEvent(QWheelEvent *_event);

private slots:
    // This is called by the QTimer
    void updateSimulation();

public :
    /**
     * @brief Constructor for GLWindow
     * @param [in] parent the parent window to create the GL context in
     */
    GLWindow(QWidget *_parent);
    ~GLWindow();
    void setTimerInterval(int _interval) { m_timer->setInterval(_interval); }

public slots:
    void toggleSimulation(bool _pressed);
    void setDrawCells(bool _pressed);
    void setDrawCollisionRadius(bool _pressed);
    void setDrawVelocityVector(bool _pressed);
    void setDrawVisionRadius(bool _pressed);
    void setDrawStrength(bool _pressed);
    void setDummyIndex(int _index);
    void setShader(int _index);
    void setStep(float _step) { m_crowdEngine.setStep(_step); }
    void setFriction(float _friction) { m_crowdEngine.setFriction(_friction);}
    void rearrangeCellPartition(int _cellSize);
    void loadBrains();
    void loadCrowds();
    void restart();
    void clear();
    void scaleCollisionRadius(double _scale);

protected:

    void initializeGL();
    void resizeGL(const int _w,const int _h);
    void paintGL();

};

#endif
