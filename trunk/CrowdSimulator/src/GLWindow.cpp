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


const static float INCREMENT=0.01;

const static float ZOOM=0.1;

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
  m_rotate=false;
  m_spinXFace=0;
  m_spinYFace=0;

}

GLWindow::~GLWindow()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"GLWindow: Shutting down NGL; removing VAO's and Shaders" << std::endl;
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
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    //LOAD PHONG SHADER
    shader->createShaderProgram("Phong");
    shader->attachShader("PhongVertex",ngl::VERTEX);
    shader->attachShader("PhongFragment",ngl::FRAGMENT);
    shader->loadShaderSource("PhongVertex","shaders/Phong.vs");
    shader->loadShaderSource("PhongFragment","shaders/Phong.fs");
    shader->compileShader("PhongVertex");
    shader->compileShader("PhongFragment");
    shader->attachShaderToProgram("Phong","PhongVertex");
    shader->attachShaderToProgram("Phong","PhongFragment");
    shader->bindAttribute("Phong",0,"inVert");
    shader->bindAttribute("Phong",1,"inUV");
    shader->bindAttribute("Phong",2,"inNormal");

    // now we have associated this data we can link the shader
    shader->linkProgramObject("Phong");
    // and make it active ready to load values
    //(*shader)["Phong"]->use();
    //shader->setShaderParam1i("Normalize",1);

    //LOAD COLOUR SHADER
    shader->createShaderProgram("Colour");
    shader->attachShader("ColourVertex",ngl::VERTEX);
    shader->attachShader("ColourFragment",ngl::FRAGMENT);
    shader->loadShaderSource("ColourVertex","shaders/Colour.vs");
    shader->loadShaderSource("ColourFragment","shaders/Colour.fs");
    shader->compileShader("ColourVertex");
    shader->compileShader("ColourFragment");
    shader->attachShaderToProgram("Colour","ColourVertex");
    shader->attachShaderToProgram("Colour","ColourFragment");
    shader->bindAttribute("Colour",0,"inVert");
    shader->linkProgramObject("Colour");

    (*shader)["Phong"]->use();

  ngl::Vec3 from(0,0,6);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // now load to our new camera
  m_cam= new ngl::Camera(from,to,up,ngl::PERSPECTIVE);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam->setShape(45,(float)720.0/576.0,0.05,350,ngl::PERSPECTIVE);
  shader->setShaderParam3f("viewerPos",m_cam->getEye().m_x,m_cam->getEye().m_y,m_cam->getEye().m_z);
  // now create our light this is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  ngl::Mat4 iv=m_cam->getViewMatrix();
  iv.transpose();
  m_light = new ngl::Light(ngl::Vec3(-1,2,1),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT );
  m_light->setTransform(iv);
  // load these values to the shader as well
  m_light->loadToShader("light");

}


void GLWindow::resizeGL(int _w, int _h)
{
  glViewport(0,0,_w,_h);
  m_cam->setShape(45,(float)_w/_h,0.05,350,ngl::PERSPECTIVE);
}
inline void GLWindow::loadMatricesToShader(ngl::TransformStack &_tx)
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=_tx.getCurrAndGlobal().getMatrix();
  MV=  _tx.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
  MVP= M*m_cam->getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
}

inline void GLWindow::loadMVPToShader(ngl::TransformStack &_tx)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    ngl::Mat4 MVP;

    MVP = _tx.getCurrAndGlobal().getMatrix()*m_cam->getVPMatrix();
    shader->setShaderParamFromMat4("MVP",MVP);
}



void GLWindow::paintGL()
{

    // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Phong"]->use();

  //(*shader)["nglColourShader"]->use();
  //shader->setShaderParam4f("Colour",1,0,0,1);

  // Rotation based on the mouse position for our global transform
  ngl::Transformation trans;
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  ngl::Mat4 final=rotY*rotX;
  // add the translations
  //  final.m_m[3][0] = m_modelPos.m_x;
  //  final.m_m[3][1] = m_modelPos.m_y;
  //  final.m_m[3][2] = m_modelPos.m_z;
  // set this in the TX stack
  trans.setMatrix(final);
  m_transformStack.setGlobal(trans);

  //move the hair
  ngl::Vec3 pos = ngl::Vec3(m_modelPos.m_x,m_modelPos.m_y,m_modelPos.m_z);
  m_hair.setPosition(pos);

   // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();


  //drawing the hair
  Particle* current;
  Particle* next;
  ngl::Vec3 distance;
  float xRot, zRot;
  std::vector<Particle*>::iterator endParticle = m_hairParticles.end();
  std::vector<Particle*>::iterator currentParticle;
  for( currentParticle = m_hairParticles.begin(); currentParticle<endParticle; ++currentParticle)
  {
      current = *currentParticle;
      next = current->getNext();

      m_transformStack.pushTransform();
      //POSITION
      m_transformStack.setPosition(current->getPosition());

      if (next)
      {
        //SCALE: distance
        distance = next->getPosition()-current->getPosition();
        m_transformStack.setScale(1,1,distance.length());
        //ROTATION
        //going from the particle
        m_transformStack.setRotation(-90,0,0);

        //DO NOT TOUCH THE ROTATION
        //rotation in X
        xRot = atan(distance.m_z/distance.m_y) * 180/M_PI;
        if (distance.m_y>0)
            zRot += 180;
        //rotation in Z
        zRot = atan(-distance.m_x/distance.m_y) * 180/M_PI;
        if (distance.m_y>0)
            zRot += 180;

       m_transformStack.addRotation(xRot,0,zRot);
      }

      shader->setShaderParam4f("Colour",1.0,0.8,0.0,1.0);
      loadMatricesToShader(m_transformStack);
      //loadMVPToShader(m_transformStack);

      if (next)
          prim->draw("hair");

      if (m_seeParticles)
      {
        m_transformStack.setScale(1,1,1);
        shader->setShaderParam4f("Colour",1.0,0.0,0.0,1.0);
        loadMatricesToShader(m_transformStack);
        prim->draw("particle");
      }
      m_transformStack.popTransform();

      //std::cout << m_hairParticles.size() << std::endl;
      //(*currentParticle)->info();

  }

}

void GLWindow::mouseMoveEvent(QMouseEvent * _event)
{
    if(_event->buttons()==Qt::LeftButton &&  m_rotate)
    {
        int diffx=_event->x()-m_origX;
        int diffy=_event->y()-m_origY;
        m_spinXFace += (float) 0.5f * diffy;
        m_spinYFace += (float) 0.5f * diffx;
        m_origX = _event->x();
        m_origY = _event->y();
        updateGL();
  }
  else if(_event->buttons() == Qt::RightButton && m_translate)
  {
        int diffX = (int)(_event->x() - m_origXPos);
        int diffY = (int)(_event->y() - m_origYPos);
        m_origXPos=_event->x();
        m_origYPos=_event->y();
        m_modelPos.m_x += INCREMENT * diffX;
        m_modelPos.m_y -= INCREMENT * diffY;
        updateGL();
    }
}


void GLWindow::mousePressEvent(QMouseEvent * _event)
{
    if(_event->button() == Qt::RightButton)
    {
        m_origX = _event->x();
        m_origY = _event->y();
        m_rotate =true;
    }
    else if(_event->button() == Qt::LeftButton)
    {
        m_origXPos = _event->x();
        m_origYPos = _event->y();
        m_translate=true;
    }

}


void GLWindow::mouseReleaseEvent(QMouseEvent * _event)
{
    if (_event->button() == Qt::RightButton)
    {
        m_rotate=false;
    }
    if (_event->button() == Qt::LeftButton)
    {
        m_translate=false;
    }
}


void GLWindow::wheelEvent(QWheelEvent *_event)
{
    if(_event->delta() > 0)
    {
        m_modelPos.m_z+=ZOOM;
    }
    else if(_event->delta() <0 )
    {
        m_modelPos.m_z-=ZOOM;
    }
    updateGL();
}


void GLWindow::timerEvent(QTimerEvent *_event)
{
    //CHICHA!!
    (void) _event;
    m_hair.applyForce(m_gravity);
    m_hair.applyRandomGravity();
    m_hair.applyWind(m_windMagnitude*m_windDirection);
    m_hair.update();
    updateGL();
}

void GLWindow::startSimTimer()
{
    m_timer=startTimer(m_timerValue);
}

void GLWindow::update()
{
    m_hair.createHair();
    m_hairParticles = m_hair.getParticles();
}

