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

const QString GLWindow::s_brainsPath = "brains";
const QString GLWindow::s_crowdsPath = "crowds";
const float GLWindow::s_rotationIncrement = 0.5;
const float GLWindow::s_translationIncrement = 0.05;
const float GLWindow::s_zoomIncrement = 1;
const int GLWindow::s_groundSize = 100;

//timer = 20 ms => 50 fps ~ 48
const int GLWindow::s_timerValue = 0;

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

    // the default might be 1, but for human fits better narrower
    m_collisionRadiusScale = 0.5;
    m_drawCollisionRadius = false;
    m_drawCells = true;
    m_drawVelocityVector = false;
    m_drawVisionRadius = false;
    m_drawStrength = false;

    //PARSER
    m_parser = new TXTParser();

    //ADD CELLPARTITION TO THE CROWDENGINE!
    m_crowdEngine.setCellPartition(new QuadraticGridCP(2));

    //ADD PHYSICS ENGINE TO THE CROWDENGINE!
    m_crowdEngine.setPhysicsEngine(new RadialPE());

    // PLAYING WITH AGENTS
    //m_crowdEngine.loadBrain("warrior");
    //m_crowdEngine.loadBrain("boid");

    //m_crowdEngine.createRandomFlock(10,10,ngl::Vec2(0,20),"flock1",2,"army1");
    //m_crowdEngine.loadBrain("warrior");
    //m_crowdEngine.createRandomFlock(10,10,ngl::Vec2(0,-20),"flock2",1,"army2");

    //Captain 1
    /*
    Agent *captain1 = new Agent();
    m_crowdEngine.loadBrain("captain");
    captain1->setBrain("warrior");
    captain1->addAttribute("flock","flock1");
    captain1->addAttribute("army","army1");
    captain1->setMaxStrength(2);
    captain1->setMaxSpeed(10);
    captain1->setPosition(ngl::Vec3(1,0,18));
    captain1->setVisionRadius(8);
    captain1->setState("holdCaptain");
    m_crowdEngine.addAgent(captain1);*/

    //Captain 2
    /*
    Agent *captain2 = new Agent();
    m_crowdEngine.loadBrain("captain");
    captain2->setBrain("captain");
    captain2->addAttribute("flock","flock2");
    captain2->addAttribute("army","army2");
    captain2->setMaxStrength(2);
    captain2->setMaxSpeed(1);
    captain2->setPosition(ngl::Vec3(-1,0,-18));
    captain2->setVisionRadius(8);
    captain2->setState("holdCaptain");
    m_crowdEngine.addAgent(captain2);
    */

    //m_crowdEngine.addAgent(troll);

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

    // PRIMITIVES FOR GUIDELINES
    m_primitives = ngl::VAOPrimitives::instance();
    m_primitives->createLineGrid("ground",s_groundSize, s_groundSize, s_groundSize);
    m_primitives->createCylinder("collisionRadius",1,1,16,1);
    m_primitives->createTorus("visionRadius",0.01,1,3,16);
    m_primitives->createCylinder("vectorModulus",0.04,2,6,1);
    m_primitives->createCone("vectorSense",0.1,0.4,6,1);

    // BOID VAO
    buildBoidVAO();

    // DUMMIES
    // The index 0 is reserved for the boidVAO
    ngl::Obj *obj;

    obj = new ngl::Obj("dummies/legoman.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies.push_back(obj);

    obj = new ngl::Obj("dummies/human.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies.push_back(obj);

    obj = new ngl::Obj("dummies/teddy.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies.push_back(obj);

    obj = new ngl::Obj("dummies/cow.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies.push_back(obj);

    obj = new ngl::Obj("dummies/speedboat.obj");
    obj->createVAO();
    obj->calcBoundingSphere();
    m_dummies.push_back(obj);

    //DEFAULT LEGOMAN
    m_dummyIndex = 1;

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
        ngl::Transformation transform;
        int cellSize;
        cellSize = m_crowdEngine.getCellSize();
        transform.setScale(cellSize,1,cellSize);
        m_transformStack.setCurrent(transform);
        loadMatricesToShader(m_transformStack);
        m_shader->setShaderParam4f("Colour",1,1,1,1);
        m_primitives->draw("ground");
    }

    //DRAWING AGENTS
    std::vector<Agent*>::const_iterator endAgent = m_crowdEngine.getAgentsEnd();
    std::vector<Agent*>::const_iterator currentAgent;
    Agent* agent;

    for(currentAgent = m_crowdEngine.getAgentsBegin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;

        //agent->print();
        m_transformStack.setCurrent(agent->getTransform());
        setStateColour(agent->getState());
        loadMatricesToShader(m_transformStack);
        if (m_dummyIndex == 0)
        {
            m_boidVAO->bind();
            m_boidVAO->draw();
            m_boidVAO->unbind();
        }
        else
        {
            m_dummies[m_dummyIndex-1]->draw();
        }
        if (m_drawCollisionRadius)
            drawCollisionRadius(agent->getCollisionRadius());
        if (m_drawVelocityVector)
            drawVector(agent->getVelocity());
        if (m_drawVisionRadius)
            drawRadius(agent->getVisionRadius());
        if (m_drawStrength)
            drawStrength(agent->getStrength(),agent->getMass());
    }

    //DRAWING TEXT WITH THE FPS
    QString text = QString("%1 FPS").arg(m_fps);
    m_text->renderText(20,10,text);



}

inline void GLWindow::drawCollisionRadius(float _collisionRadius)
{
    if (_collisionRadius>0)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        m_transformStack.setScale(_collisionRadius,_collisionRadius,2);
        m_transformStack.setRotation(90,0,0);
        loadMatricesToShader(m_transformStack);
        m_primitives->draw("collisionRadius");
        m_transformStack.setScale(1,1,1);
        m_transformStack.setRotation(-90,0,0);
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
        m_transformation.addPosition(-_strength/2.0,-_mass*scaleHeight,0);
        m_transformStack.setScale(1,1,1);
        m_transformStack.setRotation(0,-90,0);
    }
}

inline void GLWindow::drawVector(ngl::Vec4 _vector)
{
    float magnitude = _vector.length();

    // without this check we get ngl determinat 0
    // because of the scale
    if (magnitude>0)
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

        //UNDO POSITION
        m_transformStack.addPosition(-2*scale*_vector.m_x,0,-2*scale*_vector.m_z);
    }

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
        m_shader->setShaderParam4f("Colour",0,0,0,0);

    //captain states
    else if (_state=="captainRun")
        m_shader->setShaderParam4f("Colour",0,1,0,1);
    else if (_state=="captainAttack")
        m_shader->setShaderParam4f("Colour",1,0,0,1);
    else if (_state=="captainDefend")
        m_shader->setShaderParam4f("Colour",0,0,1,1);
    else if (_state=="captainDead")
        m_shader->setShaderParam4f("Colour",0,0,0,0);
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

void GLWindow::setDrawCells(bool _pressed)
{
    m_drawCells = _pressed;
    updateGL();
}

void GLWindow::setDrawCollisionRadius(bool _pressed)
{
    m_drawCollisionRadius = _pressed;
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

void GLWindow::setDummyIndex(int _index)
{
    m_dummyIndex = _index;
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
        if ( m_parser->loadCrowd((*currentFilename).toStdString(), agents) )
        {
            m_crowdEngine.addAgents(agents);
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

void GLWindow::scaleCollisionRadius(double _scale)
{
    m_collisionRadiusScale = _scale;
    m_crowdEngine.scaleCollisionRadius(_scale);
    updateGL();
}
