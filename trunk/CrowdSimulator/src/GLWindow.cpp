#include "GLWindow.h"
#include <iostream>
#include "ngl/Camera.h"
#include "ngl/Light.h"
#include "ngl/Transformation.h"
#include "ngl/TransformStack.h"
#include "ngl/Material.h"
#include "ngl/NGLInit.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/ShaderLib.h"

const QString GLWindow::s_dummiesPath = "dummies";
const QString GLWindow::s_brainsPath = "brains";
const QString GLWindow::s_crowdsPath = "crowds";
const float GLWindow::s_rotationIncrement = 0.5;
const float GLWindow::s_translationIncrement = 0.05;
const float GLWindow::s_zoomIncrement = 1;

/**
 * in this ctor we need to call the CreateCoreGLContext class, this is mainly for the MacOS Lion version as
 * we need to init the OpenGL 3.2 sub-system which is different than other platforms
 */

GLWindow::GLWindow(QWidget *_parent): QGLWidget( new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent )
{

    // set this widget to have the initial keyboard focus
    setFocus();
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    this->resize(_parent->size());

    // Now set the initial GLWindow attributes to default values
    m_rotate = false;
    m_translate = false;

    m_previousMousePosition.first = 0;
    m_previousMousePosition.second = 0;

    m_wireframeMode = false;

    m_drawBoundingBox = true;
    m_drawAxis = true;
    m_axisScale = 1;
    m_drawCells = true;
    // the default might be 1, but for human fits better narrower
    m_collisionRadiusScale = 0.5;
    m_drawCollisionRadius = false;
    m_drawVelocityVector = false;
    m_drawVisionRadius = false;
    m_drawStrength = false;
    m_drawStateColour = false;

    m_customDummy = true;

    //PARSER
    m_parser = new TXTParser();

    //ADD CELLPARTITION TO THE CROWDENGINE!
    m_crowdEngine.setCellPartition(new QuadraticGridCP(4));
    m_groundSteps = 100/4;

    //ADD PHYSICS ENGINE TO THE CROWDENGINE!
    m_crowdEngine.setPhysicsEngine(new RadialPE());

    //TIMER FOR THE UPDATING OF THE CROWDENGINE
    m_updateCrowdEngineTimer = new QTimer(this);
    connect(m_updateCrowdEngineTimer,SIGNAL(timeout()),this,SLOT(updateSimulation()));
    m_updateCrowdEngineTimer->setInterval(20);

    //TIMER FOR THE UPDATING OF THE FPS
    m_updateFPSTimer = new QTimer(this);
    connect(m_updateFPSTimer,SIGNAL(timeout()),this,SLOT(updateFPS()));
    m_updateFPSTimer->setInterval(1000);
    m_updateFPSTimer->start();
    m_fps = 0;
    m_frameCounter = 0;

}

GLWindow::~GLWindow()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"GLWindow: Quitting NGL" << std::endl;
  Init->NGLQuit();
}

void GLWindow::initializeGL()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    ngl::NGLInit *Init = ngl::NGLInit::instance();
    Init->initGlew();

    #ifdef WIN32
        glewInit();
    #endif

    // INITIALIZING SHADERS
    m_shader=ngl::ShaderLib::instance();

    //LOAD PHONG SHADER
    m_shader->createShaderProgram("Phong");
    m_shader->attachShader("PhongVertex",ngl::VERTEX);
    m_shader->attachShader("PhongFragment",ngl::FRAGMENT);
    m_shader->loadShaderSource("PhongVertex","shaders/Phong.vs");
    m_shader->loadShaderSource("PhongFragment","shaders/Phong.fs");
    m_shader->compileShader("PhongVertex");
    m_shader->compileShader("PhongFragment");
    m_shader->attachShaderToProgram("Phong","PhongVertex");
    m_shader->attachShaderToProgram("Phong","PhongFragment");
    m_shader->bindAttribute("Phong",0,"inVert");
    m_shader->bindAttribute("Phong",1,"inUV");
    m_shader->bindAttribute("Phong",2,"inNormal");

    // now we have associated this data we can link the m_shader
    m_shader->linkProgramObject("Phong");
    // and make it active ready to load values
    //(*shader)["Phong"]->use();
    //shader->setShaderParam1i("Normalize",1);

    //LOAD COLOUR SHADER
    m_shader->createShaderProgram("Colour");
    m_shader->attachShader("ColourVertex",ngl::VERTEX);
    m_shader->attachShader("ColourFragment",ngl::FRAGMENT);
    m_shader->loadShaderSource("ColourVertex","shaders/Colour.vs");
    m_shader->loadShaderSource("ColourFragment","shaders/Colour.fs");
    m_shader->compileShader("ColourVertex");
    m_shader->compileShader("ColourFragment");
    m_shader->attachShaderToProgram("Colour","ColourVertex");
    m_shader->attachShaderToProgram("Colour","ColourFragment");
    m_shader->bindAttribute("Colour",0,"inVert");
    m_shader->linkProgramObject("Colour");

    //INITIALIZE TEXT FOR DRAWING FPS ON SCREEN
    m_text = new ngl::Text(QFont("Arial",18));
    m_text->setColour(1,1,0);

    //BY DEFAULT PHONG (text loads nglTextShader)
    m_shaderIndex = phong;
    (*m_shader)["Phong"]->use();

    //CAMERA
    ngl::Vec3 from(0,10,10);
    ngl::Vec3 to(0,0,0);
    ngl::Vec3 up(0,1,0);
    m_camera = ngl::Camera(from,to,up,ngl::PERSPECTIVE);
    m_camera.setShape(45,(float)720.0/576.0,0.05,350,ngl::PERSPECTIVE);
    m_shader->setShaderParam3f("viewerPos",m_camera.getEye().m_x,m_camera.getEye().m_y,m_camera.getEye().m_z);

    //LIGHT
    ngl::Mat4 iv=m_camera.getViewMatrix();
    iv.transpose();
    m_light = ngl::Light(ngl::Vec3(-10,10,10),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT );
    m_light.setTransform(iv);
    // load these values to the shader as well
    m_light.loadToShader("light");

    // BOUNDING BOX
    m_boundingBox = new ngl::BBox(ngl::Vec3(0,0,0),1,1,1);

    // PRIMITIVES FOR GUIDELINES
    m_primitives = ngl::VAOPrimitives::instance();
    m_primitives->createLineGrid("ground",m_groundSteps, m_groundSteps, m_groundSteps);
    m_primitives->createCylinder("collisionRadius",1,1,16,1);
    m_primitives->createTorus("visionRadius",0.01,1,3,16);
    m_primitives->createCylinder("vectorModulus",0.04,1,6,1);
    m_primitives->createCone("vectorSense",0.1,0.4,6,1);

    // BOID VAO
    buildBoidVAO();

    // DUMMIES
    ngl::Obj *obj;

    std::cout << "GLWindow: Loading dummies" << std::endl;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/legoman.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["legoman"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/human.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["human"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/teddy.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies ["teddy"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/minion.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["minion"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/cow.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["cow"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/speedboat.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["speedboat"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/r2d2.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["r2d2"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/droid.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["droid"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/aragorn.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["aragorn"] = obj;

    obj = new ngl::Obj(s_dummiesPath.toStdString()+"/legolas.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies["legolas"] = obj;

    //DEFAULT LEGOMAN
    m_currentDummy = m_dummies.at("legoman");

}

void GLWindow::buildBoidVAO()
{
    //BUILD SIMPLE BOID FOR EFFICIENT DRAWING
    ngl::Vec3 vertex[] =
    {
        ngl::Vec3(0,0.5,0),
        ngl::Vec3(1,0,0),
        ngl::Vec3(0,0,-0.5),

        ngl::Vec3(0,0.5,0),
        ngl::Vec3(1,0,0),
        ngl::Vec3(0,0,0.5),

        ngl::Vec3(0,0.5,0),
        ngl::Vec3(-0.5,0,0),
        ngl::Vec3(0,0,-0.5),

        ngl::Vec3(0,0.5,0),
        ngl::Vec3(-0.5,0,0),
        ngl::Vec3(0,0,0.5)
    };
    m_boidVAO = ngl::VertexArrayObject::createVOA(GL_TRIANGLES);
    m_boidVAO->bind();
    m_boidVAO->setData(sizeof(vertex),vertex[0].m_x);
    m_boidVAO->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
    m_boidVAO->setNumIndices(sizeof(vertex)/sizeof(ngl::Vec3));
    m_boidVAO->unbind();
}


void GLWindow::resizeGL(int _w, int _h)
{
    glViewport(0,0,_w,_h);
    m_camera.setShape(45,(float)_w/_h,0.05,350,ngl::PERSPECTIVE);
    //THIS LINE GAVE ME A LOT OF PAIN!
    m_text->setScreenSize(_w,_h);
}

inline void GLWindow::loadMatricesToShader(ngl::TransformStack &_tx)
{
    ngl::Mat4 M;
    ngl::Mat4 MV;
    ngl::Mat4 MVP;

    if (m_shaderIndex==phong)
    {

        ngl::Mat3 normalMatrix;
        M=_tx.getCurrAndGlobal().getMatrix();
        MV= _tx.getCurrAndGlobal().getMatrix()*m_camera.getViewMatrix();
        MVP= M*m_camera.getVPMatrix();
        normalMatrix=MV;
        normalMatrix.inverse();
        m_shader->setShaderParamFromMat4("MV",MV);
        m_shader->setShaderParamFromMat4("MVP",MVP);
        m_shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
        m_shader->setShaderParamFromMat4("M",M);
    }
    else if (m_shaderIndex==colour)
    {
        M=_tx.getCurrAndGlobal().getMatrix();
        MV= _tx.getCurrAndGlobal().getMatrix()*m_camera.getViewMatrix();
        MVP= M*m_camera.getVPMatrix();
        m_shader->setShaderParamFromMat4("MVP",MVP);
    }
}

void GLWindow::paintGL()
{

    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enabling depth test
    glEnable(GL_DEPTH_TEST);

    //DRAWING
    /**
     * The shader needs to be set each time
     * because the ngl::text leaves nglTextShader
     */
    if (m_shaderIndex==phong)
        (*m_shader)["Phong"]->use();
    else if (m_shaderIndex==colour)
        (*m_shader)["Colour"]->use();

    //Drawing the grid
    if (m_drawCells)
    {
        //ngl::Transformation transform;
        int cellSize;
        cellSize = m_crowdEngine.getCellSize();
        m_transformStack.setScale(cellSize,1,cellSize);
        loadMatricesToShader(m_transformStack);
        m_shader->setShaderParam4f("Colour",1,1,1,1);
        m_primitives->draw("ground");
        m_transformStack.setScale(1,1,1);
    }

    //Drawing axis
    if (m_drawAxis)
    {
        ngl::Vec4 vector;
        // X
        vector = ngl::Vec4(1,0,0,0);
        m_shader->setShaderParam4f("Colour",1,0,0,1);
        drawVector(vector,m_axisScale);
        // Y
        vector = ngl::Vec4(0,1,0,0);
        m_shader->setShaderParam4f("Colour",0,1,0,1);
        drawVector(vector,m_axisScale);
        // Z
        vector = ngl::Vec4(0,0,1,0);
        m_shader->setShaderParam4f("Colour",0,0,1,1);
        drawVector(vector,m_axisScale);

    }

    //Drawing the bounding box
    if (m_drawBoundingBox)
    {
        float boundingBoxSize = m_crowdEngine.getBoundingBoxSize();
        m_transformStack.setScale(boundingBoxSize,boundingBoxSize,boundingBoxSize);
        loadMatricesToShader(m_transformStack);
        m_shader->setShaderParam4f("Colour",1,1,1,1);
        m_boundingBox->draw();
    }

    //DRAWING AGENTS
    std::vector<Agent*>::const_iterator endAgent = m_crowdEngine.getAgentsEnd();
    std::vector<Agent*>::const_iterator currentAgent;
    Agent* agent;

    for(currentAgent = m_crowdEngine.getAgentsBegin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        //agent->print();

        //TRANSFORMATION
        m_transformStack.setCurrent(agent->getTransform());
        loadMatricesToShader(m_transformStack);

        //COLOUR
        if (m_drawStateColour)
            setStateColour(agent->getState());
        else
            m_shader->setShaderParam4f("Colour",1,1,1,1);

        //DUMMY
        if (m_customDummy)
            agent->getDummy()->draw();
        else if (m_currentDummy)
            m_currentDummy->draw();
        else
        {
            m_boidVAO->bind();
            m_boidVAO->draw();
            m_boidVAO->unbind();
        }

        if (m_drawCollisionRadius)
            drawCollisionRadius(agent->getCollisionRadius());
        if (m_drawVelocityVector)
            drawVector(agent->getVelocity(),2);
        if (m_drawVisionRadius)
            drawRadius(agent->getVisionRadius());
        if (m_drawStrength)
            drawStrength(agent->getStrength(),agent->getMass());
    }

    m_transformStack.setPosition(0,0,0);
    m_transformStack.setRotation(0,0,0);
    m_transformStack.setScale(1,1,1);

    //DRAWING TEXT WITH THE FPS
    QString text = QString("%1 FPS").arg(m_fps);
    m_text->renderText(20,10,text);

}

inline void GLWindow::drawCollisionRadius(float _collisionRadius)
{
    if (_collisionRadius>0)
    {
        if (!m_wireframeMode)
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        m_transformStack.setScale(_collisionRadius,_collisionRadius,2);
        m_transformStack.setRotation(90,0,0);
        loadMatricesToShader(m_transformStack);
        m_primitives->draw("collisionRadius");
        m_transformStack.setScale(1,1,1);
        m_transformStack.setRotation(-90,0,0);
        if (!m_wireframeMode)
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
}

inline void GLWindow::drawStrength(float _strength, float _mass)
{
    if (_strength>0)
    {
        float scaleLength=1;
        float scaleHeight=2.2;
        m_transformStack.addPosition(-0.02,_mass*scaleHeight,0);
        m_transformStack.setScale(0.05,0.1,_strength*scaleLength);
        m_transformStack.setRotation(0,90,0);
        loadMatricesToShader(m_transformStack);
        m_primitives->draw("cube");
        m_transformStack.addPosition(0.02,-_mass*scaleHeight,0);
        m_transformStack.setScale(1,1,1);
        m_transformStack.setRotation(0,-90,0);
    }
}

inline void GLWindow::drawVector(ngl::Vec4 _vector, float _scale)
{
    float magnitude = _vector.length();

    float xRot=0;
    float yRot=0;
    //_scale = 2;

    xRot = atan2(_vector.m_y,sqrt(_vector.m_x*_vector.m_x+_vector.m_z*_vector.m_z)) * (180/M_PI);
    yRot = atan2(-_vector.m_z,_vector.m_x) * 180/M_PI;

    m_transformStack.setRotation(xRot,yRot-90,0);

    //MODULUS
    // without this check we get ngl determinat 0
    // because of the scale
    if (magnitude>0 && _scale>0)
    {
        m_transformStack.setScale(1,1,_scale*_vector.length());
        loadMatricesToShader(m_transformStack);
        m_primitives->draw("vectorModulus");
    }

    //SENSE
    m_transformStack.setScale(1,1,-1);
    m_transformStack.addPosition(_scale*_vector.m_x,_scale*_vector.m_y,_scale*_vector.m_z);
    loadMatricesToShader(m_transformStack);
    m_primitives->draw("vectorSense");

    //UNDO POSITION
    m_transformStack.addPosition(-_scale*_vector.m_x,-_scale*_vector.m_y,-_scale*_vector.m_z);
}

inline void GLWindow::drawRadius(float _radius)
{
    m_transformStack.setScale(_radius,_radius,1);
    m_transformStack.setRotation(90,0,0);
    loadMatricesToShader(m_transformStack);
    m_primitives->draw("visionRadius");
    m_transformStack.setScale(1,1,1);
    m_transformStack.setRotation(-90,0,0);

}

inline void GLWindow::setStateColour(std::string _state)
{
    //warrior states
    if (_state=="warriorRun")
        m_shader->setShaderParam4f("Colour",0,1,0,1);
    else if (_state=="warriorAttack")
        m_shader->setShaderParam4f("Colour",1,0,0,1);
    else if (_state=="warriorDefend")
        m_shader->setShaderParam4f("Colour",0,0,1,1);
    else if (_state=="warriorDead")
        m_shader->setShaderParam4f("Colour",0,0,0,1);

    //captain states
    else if (_state=="captainRun")
        m_shader->setShaderParam4f("Colour",0,1,0,1);
    else if (_state=="captainAttack")
        m_shader->setShaderParam4f("Colour",1,0,0,1);
    else if (_state=="captainDefend")
        m_shader->setShaderParam4f("Colour",0,0,1,1);
    else if (_state=="captainDead")
        m_shader->setShaderParam4f("Colour",0,0,0,1);

    //shooter states
    else if (_state=="shooterRun")
        m_shader->setShaderParam4f("Colour",0,1,0,1);
    else if (_state=="shooterShoot")
        m_shader->setShaderParam4f("Colour",1,0,0,1);
    else if (_state=="shooterGoBack")
        m_shader->setShaderParam4f("Colour",0,0,1,1);

    //target states
    else if (_state=="targetDead")
        m_shader->setShaderParam4f("Colour",0,0,0,1);

    //default colour
    else
        m_shader->setShaderParam4f("Colour",1,1,1,1);
}

void GLWindow::mouseMoveEvent(QMouseEvent * _event)
{
    bool rotate;

    if( (rotate = _event->buttons()==Qt::LeftButton &&  m_rotate ) ||
        (_event->buttons() == Qt::RightButton && m_translate ) )
    {
        if (rotate)
        {
            m_globalRotation.m_x += s_rotationIncrement * (_event->y() - m_previousMousePosition.second);
            m_globalRotation.m_y += s_rotationIncrement * (_event->x() - m_previousMousePosition.first);
            m_previousMousePosition.first = _event->x();
            m_previousMousePosition.second = _event->y();
        }
        else
        {
            m_globalTranslation.m_x += s_translationIncrement * (_event->x() - m_previousMousePosition.first);
            m_globalTranslation.m_y -= s_translationIncrement * (_event->y() - m_previousMousePosition.second);
            m_previousMousePosition.first = _event->x();
            m_previousMousePosition.second = _event->y();

        }
        ngl::Transformation globalTransform;
        ngl::Mat4 xRotMat, yRotMat, globalMatrix;
        xRotMat.rotateX(m_globalRotation.m_x);
        yRotMat.rotateY(m_globalRotation.m_y);
        globalMatrix = yRotMat*xRotMat;
        globalMatrix.m_m[3][0] = m_globalTranslation.m_x;
        globalMatrix.m_m[3][1] = m_globalTranslation.m_y;
        globalMatrix.m_m[3][2] = m_globalTranslation.m_z;
        globalTransform.setMatrix(globalMatrix);
        m_transformStack.setGlobal(globalTransform);

        updateGL();
  }
}


void GLWindow::mousePressEvent(QMouseEvent * _event)
{
    if(_event->button() == Qt::LeftButton)
    {
        m_previousMousePosition.first = _event->x();
        m_previousMousePosition.second = _event->y();
        m_rotate =true;
    }
    else if(_event->button() == Qt::RightButton)
    {
        m_previousMousePosition.first = _event->x();
        m_previousMousePosition.second = _event->y();
        m_translate=true;
    }

}

void GLWindow::mouseReleaseEvent(QMouseEvent * _event)
{
    if (_event->button() == Qt::LeftButton)
    {
        m_rotate=false;
    }
    if (_event->button() == Qt::RightButton)
    {
        m_translate=false;
    }
}

void GLWindow::wheelEvent(QWheelEvent *_event)
{
    if(_event->delta() > 0)
    {
        m_globalTranslation.m_y += s_zoomIncrement;
        m_globalTranslation.m_z += s_zoomIncrement;
    }
    else if(_event->delta() <0 )
    {
        m_globalTranslation.m_y -= s_zoomIncrement;
        m_globalTranslation.m_z -= s_zoomIncrement;
    }
    ngl::Transformation globalTransform;
    ngl::Mat4 xRotMat, yRotMat, globalMatrix;
    xRotMat.rotateX(m_globalRotation.m_x);
    yRotMat.rotateY(m_globalRotation.m_y);
    globalMatrix = yRotMat*xRotMat;
    globalMatrix.m_m[3][0] = m_globalTranslation.m_x;
    globalMatrix.m_m[3][1] = m_globalTranslation.m_y;
    globalMatrix.m_m[3][2] = m_globalTranslation.m_z;
    globalTransform.setMatrix(globalMatrix);
    m_transformStack.setGlobal(globalTransform);

    updateGL();
}

void GLWindow::updateSimulation()
{
    m_crowdEngine.update();
    updateGL();
    ++m_frameCounter;
}

void GLWindow::toggleSimulation(bool _pressed)
{
    if (_pressed)
    {
        if (!m_updateCrowdEngineTimer->isActive())
            m_updateCrowdEngineTimer->start();
    }
    else
    {
        if (m_updateCrowdEngineTimer->isActive())
            m_updateCrowdEngineTimer->stop();
    }
}

void GLWindow::updateFPS()
{
    m_fps = m_frameCounter;
    m_frameCounter = 0;
    updateGL();
}

void GLWindow::setDrawAxis(bool _pressed)
{
    m_drawAxis = _pressed;
    updateGL();
}

void GLWindow::scaleAxis(double _scale)
{
    m_axisScale = _scale;
    updateGL();
}

void GLWindow::setDrawCells(bool _pressed)
{
    m_drawCells = _pressed;
    updateGL();
}

void GLWindow::setDrawBoundingBox(bool _pressed)
{
    m_drawBoundingBox = _pressed;
    updateGL();
}

void GLWindow::setBoundingBoxSize(double _size)
{
    m_crowdEngine.setBoundingBoxSize(_size);
    m_groundSteps = ceil(ceil(_size/2.0)/(float)m_crowdEngine.getCellSize()) * 2;
    m_primitives->createLineGrid("ground",m_groundSteps,m_groundSteps,m_groundSteps);
    updateGL();
}

void GLWindow::setDrawCollisionRadius(bool _pressed)
{
    m_drawCollisionRadius = _pressed;
    updateGL();
}

void GLWindow::scaleCollisionRadius(double _scale)
{
    m_collisionRadiusScale = _scale;
    m_crowdEngine.scaleCollisionRadius(_scale);
    updateGL();
}

void GLWindow::setDrawVelocityVector(bool _pressed)
{
    m_drawVelocityVector = _pressed;
    updateGL();
}

void GLWindow::setDrawVisionRadius(bool _pressed)
{
    m_drawVisionRadius = _pressed;
    updateGL();
}

void GLWindow::setDrawStrength(bool _pressed)
{
    m_drawStrength = _pressed;
    updateGL();
}

void GLWindow::setDrawStateColour(bool _pressed)
{
    m_drawStateColour = _pressed;
    updateGL();
}

void GLWindow::setCurrentDummy(int _index)
{
    if (_index == 11)
        m_customDummy = true;
    else
    {
        /**
         * If you try to set a dummy that was not
         * loaded it will throw an exception
         */
        if (_index == 0)
            m_currentDummy = NULL;
        else if (_index == 1)
            m_currentDummy = m_dummies.at("legoman");
        else if (_index == 2)
            m_currentDummy = m_dummies.at("human");
        else if (_index == 3)
            m_currentDummy = m_dummies.at("teddy");
        else if (_index == 4)
            m_currentDummy = m_dummies.at("minion");
        else if (_index == 5)
            m_currentDummy = m_dummies.at("aragorn");
        else if (_index == 6)
            m_currentDummy = m_dummies.at("legolas");
        else if (_index == 7)
            m_currentDummy = m_dummies.at("r2d2");
        else if (_index == 8)
            m_currentDummy = m_dummies.at("droid");
        else if (_index == 9)
            m_currentDummy = m_dummies.at("cow");
        else if (_index == 10)
            m_currentDummy = m_dummies.at("speedboat");
        m_customDummy = false;
    }

    updateGL();
}

void GLWindow::setShader(int _index)
{
    m_shaderIndex = _index;

    if (m_shaderIndex==phong)
        (*m_shader)["Phong"]->use();
    else if (m_shaderIndex==colour)
        (*m_shader)["Colour"]->use();

    updateGL();
}

void GLWindow::rearrangeCellPartition(int _cellSize)
{
    if (m_updateCrowdEngineTimer->isActive())
    {
        m_updateCrowdEngineTimer->stop();
        m_crowdEngine.rearrangePartition(_cellSize);
        m_updateCrowdEngineTimer->start();
    }
    else
    {
        m_crowdEngine.rearrangePartition(_cellSize);
    }
    m_groundSteps = ceil(ceil(m_crowdEngine.getBoundingBoxSize()/2.0)/(float)_cellSize) * 2;
    m_primitives->createLineGrid("ground",m_groundSteps,m_groundSteps,m_groundSteps);
    updateGL();
}

void GLWindow::loadBrains()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load Brains"), s_brainsPath, tr("Lua Files (*.lua)"));

    QStringList::iterator filenameEnd = filenames.end();
    QStringList::iterator currentFilename;
    for (currentFilename = filenames.begin(); currentFilename!=filenameEnd; ++currentFilename)
        m_crowdEngine.loadBrain((*currentFilename).toStdString());

}

void GLWindow::loadCrowds()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load Crowds"), s_crowdsPath, tr("Text Files (*.txt)"));
    std::vector<Agent*> agents;

    QStringList::iterator filenameEnd = filenames.end();
    QStringList::iterator currentFilename;
    for (currentFilename = filenames.begin(); currentFilename!=filenameEnd; ++currentFilename)
    {
        if ( m_parser->loadCrowd((*currentFilename).toStdString(), m_dummies, agents) )
        {
            m_crowdEngine.addAgents(agents);
            //YOU FORGOT THIS LINE, DUMBASS
            /**
             * Without clearing the agents, if you load more than one file at the same time
             * you add agents twice, provoking a memory corruption when deleting them twice
             */
            agents.clear();
            m_crowdEngine.scaleCollisionRadius(m_collisionRadiusScale);
        }
        else
            std::cout << "GLWindow: ERROR: Impossible to load crowd from " << (*currentFilename).toStdString() << std::endl;
    }

    updateGL();
}

void GLWindow::clear()
{
    if (m_updateCrowdEngineTimer->isActive())
    {
        m_updateCrowdEngineTimer->stop();
        m_crowdEngine.clear();
        m_updateCrowdEngineTimer->start();
    }
    else
    {
        m_crowdEngine.clear();
    }
    updateGL();
}

void GLWindow::restart()
{
    if (m_updateCrowdEngineTimer->isActive())
    {
        m_updateCrowdEngineTimer->stop();
        m_crowdEngine.restart();
        m_updateCrowdEngineTimer->start();
    }
    else
    {
        m_crowdEngine.restart();
    }
    updateGL();
}
