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
#include <ngl/Text.h>
// it must be included after our stuff becuase GLEW needs to be first
#include <QResizeEvent>
#include <QEvent>
#include <QTimer>
#include <QFileDialog>

#include "TXTParser.h"
#include "CrowdEngine.h"
#include "QuadraticGridCP.h"
#include "RadialPE.h"

enum shader{phong,colour};

class GLWindow : public QGLWidget
{

// this must include this if you use Qt signals/slots
Q_OBJECT

private :
    static const QString s_dummiesPath;
    static const QString s_brainsPath;
    static const QString s_crowdsPath;
    static const float s_rotationIncrement;
    static const float s_translationIncrement;
    static const float s_zoomIncrement;


    QTimer *m_updateCrowdEngineTimer;
    QTimer *m_updateFPSTimer;
    ngl::BBox *m_boundingBox;
    int m_groundSteps;
    int m_fps;
    int m_frameCounter;
    ngl::Text *m_text;
    bool m_rotate;
    bool m_translate;
    std::pair<int,int> m_previousMousePosition;
    ngl::Vec2 m_globalRotation;
    ngl::Vec3 m_globalTranslation;
    ngl::TransformStack m_transformStack;
    ngl::VAOPrimitives *m_primitives;
    ngl::ShaderLib *m_shader;
    int m_shaderIndex;
    ngl::Camera m_camera;
    ngl::Light m_light;
    std::map<std::string,ngl::Obj*> m_dummies;
    ngl::Obj* m_currentDummy;
    bool m_customDummy;
    ngl::VertexArrayObject *m_boidVAO;
    float m_collisionRadiusScale;
    bool m_drawBoundingBox;
    bool m_drawCollisionRadius;
    bool m_drawCells;
    bool m_drawVelocityVector;
    bool m_drawVisionRadius;
    bool m_drawStrength;
    bool m_drawAxis;
    bool m_drawStateColour;
    float m_axisScale;
    bool m_wireframeMode;

    Parser* m_parser;
    CrowdEngine m_crowdEngine;

    void buildBoidVAO();
    void loadMatricesToShader(ngl::TransformStack &_tx);
    void inline drawCollisionRadius(float _collisionRadius);
    void inline drawVector(ngl::Vec4 _vector, float _scale);
    void inline drawRadius(float _radius);
    void inline drawStrength(float _strength, float _mass);
    void inline setStateColour(std::string _state);

    void mouseMoveEvent(QMouseEvent * _event);
    void mousePressEvent(QMouseEvent *_event);
    void mouseReleaseEvent(QMouseEvent *_event);
    void wheelEvent(QWheelEvent *_event);

private slots:
    // This is called by the QTimer m_updateCrowdEngineTimer
    void updateSimulation();
    // This is called by the QTimer m_updateFPSTimer
    void updateFPS();

public :
    /**
     * @brief Constructor for GLWindow
     * @param [in] parent the parent window to create the GL context in
     */
    GLWindow(QWidget *_parent);
    ~GLWindow();
    void setTimerInterval(int _interval) { m_updateCrowdEngineTimer->setInterval(_interval); }
    void setWireframeMode(bool _wireframeMode) { m_wireframeMode = _wireframeMode; }

public slots:
    void toggleSimulation(bool _pressed);
    void setDrawAxis(bool _pressed);
    void setDrawCells(bool _pressed);
    void setDrawBoundingBox(bool _pressed);
    void setDrawCollisionRadius(bool _pressed);
    void setDrawVelocityVector(bool _pressed);
    void setDrawVisionRadius(bool _pressed);
    void setDrawStrength(bool _pressed);
    void setDrawStateColour(bool _pressed);
    void setCurrentDummy(int _index);
    void setShader(int _index);
    void setStep(float _step) { m_crowdEngine.setStep(_step); }
    void setFriction(float _friction) { m_crowdEngine.setFriction(_friction);}
    void rearrangeCellPartition(int _cellSize);
    void loadBrains();
    void loadCrowds();
    void restart();
    void clear();
    void scaleCollisionRadius(double _scale);
    void scaleAxis(double _scale);
    void setBoundingBoxSize(double _size);

protected:

    void initializeGL();
    void resizeGL(const int _w,const int _h);
    void paintGL();

};

#endif
