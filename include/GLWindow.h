#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__

/**
 * @file GLWindow.h
 * @brief Class in charge to handle the OpenGL Window and
 * pass the user input to the CrowdEngine
 */

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
#include "CylinderPE.h"
#include "SpherePE.h"

/**
 * @enum shader
 * @brief enum that identifies the current shader in use
 */
enum shader{
    SHADER_PHONG,
    SHADER_COLOUR
};

/**
 * @class GLWindow
 * @brief Class in charge to handle the OpenGL Window and
 * pass the user input to the CrowdEngine
 */
class GLWindow : public QGLWidget
{

// this must include this if you use Qt signals/slots
Q_OBJECT

private :
    /**
     * @brief Path for the dummies in the filesystem
     */
    static const QString s_dummiesPath;
    /**
     * @brief Path for the brains in the filesystem
     */
    static const QString s_brainsPath;
    /**
     * @brief Path for the agents in the filesystem
     */
    static const QString s_agentsPath;
    /**
     * @brief Rotation step when the user rotates with the mouse
     */
    static const float s_rotationIncrement;
    /**
     * @brief Translation step when the user translates with the mouse
     */
    static const float s_translationIncrement;
    /**
     * @brief Zoom step when the user makes zoom with the mouse wheel
     */
    static const float s_zoomIncrement;

    /**
     * @brief The timer for updating the crowd engine
     */
    QTimer *m_updateCrowdEngineTimer;
    /**
     * @brief The timer for checking the FPS of the system
     */
    QTimer *m_updateFPSTimer;
    /**
     * @brief Bounding box that wraps the world to be drawn
     */
    ngl::BBox *m_boundingBox;
    /**
     * @brief Steps of the grip which represents the ground.
     * This is calculated as boundingBoxSize / CellSize
     */
    int m_groundSteps;
    /**
     * @brief Current FPS of the system
     */
    int m_fps;
    /**
     * @brief Counter of the frames to calculate the FPS
     * of the system each second.
     */
    int m_frameCounter;
    /**
     * @brief Text to render onto the screen
     */
    ngl::Text *m_text;
    /**
     * @brief To know if the user is currently rotating
     */
    bool m_rotate;
    /**
     * @brief To know if the user is currently translating
     */
    bool m_translate;
    /**
     * @brief Stores the previous mouse position
     */
    std::pair<int,int> m_previousMousePosition;
    /**
     * @brief Stores the global rotation
     */
    ngl::Vec2 m_globalRotation;
    /**
     * @brief Stores the global translation
     */
    ngl::Vec3 m_globalTranslation;
    /**
     * @brief Transformation Stack which holds the transformations
     */
    ngl::TransformStack m_transformStack;
    /**
     * @brief Instance of ngl::VAOPrimitives to access to its functionalities
     */
    ngl::VAOPrimitives *m_primitives;
    /**
     * @brief Instance of ngl::ShaderLib to access to its functionalities
     */
    ngl::ShaderLib *m_shader;
    /**
     * @brief Index of the current shader in use
     */
    int m_shaderIndex;
    /**
     * @brief Stores the camera of the scene
     */
    ngl::Camera m_camera;
    /**
     * @brief Stores the light of the scene
     */
    ngl::Light m_light;
    /**
     * @brief Vector which stores the VAO's of the dummies availables
     * for representing the agents of the simulation
     */
    std::map<std::string,ngl::Obj*> m_dummies;
    /**
     * @brief Current dummy in use, in case it is no customized
     */
    ngl::Obj* m_currentDummy;
    /**
     * @brief To know if the customized dummy should be used or not
     */
    bool m_customDummy;
    /**
     * @brief VAO of a simple boid
     */
    ngl::VertexArrayObject *m_boidVAO;
    /**
     * @brief Remember the last collision radius scale used
     * in case new agents are loaded
     */
    float m_collisionRadiusScale;
    /**
     * @brief To know if the bounding box has to be drawn
     */
    bool m_drawBoundingBox;
    /**
     * @brief To know if the collision radius has to be drawn
     */
    bool m_drawCollisionRadius;
    /**
     * @brief To know if the cells have to be drawn
     */
    bool m_drawCells;
    /**
     * @brief To know if the velocity vector has to be drawn
     */
    bool m_drawVelocityVector;
    /**
     * @brief To know if the vision radius has to be drawn
     */
    bool m_drawVisionRadius;
    /**
     * @brief To know if the strength has to be drawn
     */
    bool m_drawStrength;
    /**
     * @brief To know if the axis have to be drawn
     */
    bool m_drawAxis;
    /**
     * @brief To know if the state colour code has to be drawn
     */
    bool m_drawStateColour;
    /**
     * @brief The scale value of the axis in the origin
     */
    float m_axisScale;
    /**
     * @brief To know if the wireframe mode is active
     */
    bool m_wireframeMode;

    /**
     * @brief The parser used to load agents
     */
    Parser* m_parser;
    /**
     * @brief The crowd engine used to perform the simulation
     */
    CrowdEngine m_crowdEngine;

    void buildBoidVAO();
    void loadMatricesToShader(ngl::TransformStack &_tx);
    void inline drawCollisionCylinder(float _collisionRadius);
    void inline drawCollisionSphere(float _mass, float _collisionRadius);
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
    /**
     * @brief Set the timer interval for updating the crowd engine
     */
    void setTimerInterval(int _interval) { m_updateCrowdEngineTimer->setInterval(_interval); }
    /**
     * @brief Set the wireframe mode on
     */
    void setWireframeMode(bool _wireframeMode) { m_wireframeMode = _wireframeMode; }
    /**
     * @brief Set the step for the simulation
     */
    void setStep(float _step) { m_crowdEngine.setStep(_step); }

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
    void setFriction(double _friction) { m_crowdEngine.setFriction(_friction);}
    void setGravity(double _gravity) { m_crowdEngine.setGravity(_gravity); }
    void rearrangeCellPartition(int _cellSize);
    void loadBrains();
    void loadAgents();
    void restart();
    void clear();
    void scaleCollisionRadius(double _scale);
    void scaleAxis(double _scale);
    void setBoundingBoxSize(double _size);
    void setPhysicsEngine(int _index);

protected:

    /**
     * @brief Initialize the OpenGL context
     */
    void initializeGL();
    /**
     * @brief Resize the Opengl window
     */
    void resizeGL(const int _w,const int _h);
    /**
     * @brief Render the current content of the OpenGL window
     */
    void paintGL();

};

#endif
