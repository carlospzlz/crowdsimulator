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


const float GLWindow::s_rotationIncrement = 0.5;
const float GLWindow::s_translationIncrement = 0.05;
const float GLWindow::s_zoomIncrement = 1;
const int GLWindow::s_groundSize = 100;

//timer = 20 ms => 50 fps ~ 48
const int GLWindow::s_timerValue = 20;

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
    m_simulating = true;

    m_previousMousePosition.first = 0;
    m_previousMousePosition.second = 0;

    // PLAYING WITH AGENTS
    //m_crowdEngine.loadBrain("printer");
    m_crowdEngine.loadBrain("warrior");
    m_crowdEngine.createRandomFlock(5,10,ngl::Vec2(0,0),"testFlock");

    Agent *myAgent = new Agent();
    m_crowdEngine.loadBrain("leaderBoid");
    myAgent->setBrain("leaderBoid");
    myAgent->addAttribute("flock","testFlock");
    myAgent->setVisionRadius(8);
    m_crowdEngine.addAgent(myAgent);


    //START TIMER (maybe it's not simulating)
    m_timer = startTimer(s_timerValue);

}

GLWindow::~GLWindow()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"GLWindow: removing camera, light and quitting NGL" << std::endl;
  //delete m_camera;
  //delete m_light;
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


    (*m_shader)["Phong"]->use();
    m_shader->setShaderParam4f("Colour",1,0,0,1);

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
    m_light = ngl::Light(ngl::Vec3(-1,2,1),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT );
    m_light.setTransform(iv);
    // load these values to the shader as well
    m_light.loadToShader("light");

    //Primitives for drawing
    m_primitives = ngl::VAOPrimitives::instance();
    m_primitives->createLineGrid("ground",s_groundSize, s_groundSize, s_groundSize);
    m_primitives->createTorus("radius",0.01,1,3,16);
    m_primitives->createCylinder("vectorModulus",0.04,2,6,1);
    m_primitives->createCone("vectorSense",0.1,0.4,6,1);


    //Loading geometry for testing
    m_dummy= new ngl::Obj("dummies/legoman.obj");
    m_dummy->createVAO();
    m_dummy->calcBoundingSphere();

}


void GLWindow::resizeGL(int _w, int _h)
{
    glViewport(0,0,_w,_h);
    m_camera.setShape(45,(float)_w/_h,0.05,350,ngl::PERSPECTIVE);
}

inline void GLWindow::loadMatricesToShader(ngl::TransformStack &_tx)
{

    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M=_tx.getCurrAndGlobal().getMatrix();
    MV=  _tx.getCurrAndGlobal().getMatrix()*m_camera.getViewMatrix();
    MVP= M*m_camera.getVPMatrix();
    normalMatrix=MV;
    normalMatrix.inverse();
    m_shader->setShaderParamFromMat4("MV",MV);
    m_shader->setShaderParamFromMat4("MVP",MVP);
    m_shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
    m_shader->setShaderParamFromMat4("M",M);
}

inline void GLWindow::loadMVPToShader(ngl::TransformStack &_tx)
{
    ngl::Mat4 MVP;

    MVP = _tx.getCurrAndGlobal().getMatrix()*m_camera.getVPMatrix();
    m_shader->setShaderParamFromMat4("MVP",MVP);
}

void GLWindow::paintGL()
{

    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //DRAWING
    ngl::Transformation transform;
    int cellSize;

    //Drawing the grid
    cellSize = m_crowdEngine.getCellSize();
    transform.setScale(cellSize,0,cellSize);
    m_transformStack.setCurrent(transform);
    loadMVPToShader(m_transformStack);
    m_shader->setShaderParam4f("Colour",1,1,1,1);
    m_primitives->draw("ground");

    //DRAWING AGENTS
    std::vector<Agent*>::const_iterator endAgent = m_crowdEngine.getAgentsEnd();
    std::vector<Agent*>::const_iterator currentAgent;
    Agent* agent;

    // COLOUR
    m_shader->setShaderParam4f("Colour",1,0,0,1);
    //shader->setShaderParam4f("Colour",0.5,0.5,0.5,1);

    for(currentAgent = m_crowdEngine.getAgentsBegin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        //agent->print();
        m_transformStack.setCurrent(agent->getTransform());
        loadMatricesToShader(m_transformStack);
        //primitives->draw("cube");
        m_dummy->draw();

        drawRadius(agent->getVisionRadius());

        drawVector(agent->getVelocity());
        
    }

    /*
    shader->use("Colour");

    for(currentAgent = m_crowdEngine.getAgentsBegin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        transform.setScale(2,2,1);
        transform.setPosition(agent->getPosition());
        transform.setRotation(90,0,0);
        m_transformStack.setCurrent(transform);
        loadMVPToShader(m_transformStack);

        primitives->draw("radius");

    }

    shader->use("Phong");
    */

}

inline void GLWindow::drawVector(ngl::Vec4 _vector)
{
    int scale=2;
    float yRot=0;

    yRot = atan2(-_vector.m_z,_vector.m_x) * 180/M_PI;
    m_transformStack.setRotation(0,yRot-90,0);

    //MODULUS
    m_transformStack.setScale(1,1,scale*_vector.length());
    loadMatricesToShader(m_transformStack);
    m_primitives->draw("vectorModulus");

    //SENSE
    // why x2?!!
    m_transformStack.setScale(1,1,-1);
    m_transformStack.addPosition(2*scale*_vector.m_x,0,2*scale*_vector.m_z);
    loadMatricesToShader(m_transformStack);
    m_primitives->draw("vectorSense");

}

inline void GLWindow::drawRadius(int _radius)
{
    m_transformStack.setScale(_radius,_radius,1);
    m_transformStack.setRotation(90,0,0);
    loadMatricesToShader(m_transformStack);
    m_primitives->draw("radius");

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


void GLWindow::timerEvent(QTimerEvent *_event)
{
    (void) _event;

    //CROWD SIMULATION!!
    if (m_simulating)
        m_crowdEngine.update();

    updateGL();
}
