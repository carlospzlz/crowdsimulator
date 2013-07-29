#include <PhysicsEngine.h>

PhysicsEngine::PhysicsEngine()
{
    m_boundingBoxSize = 100;
}

void PhysicsEngine::checkCollisionBoundingBox(Agent *_agent)
{
    float x,z;
    float limit;
    ngl::Vec4 velocity;
    float step = Agent::getStep();

    x = _agent->getPosition().m_x;
    z = _agent->getPosition().m_z;
    velocity = _agent->getVelocity();

    limit = m_boundingBoxSize/2.0;

    if (x<-limit || x>limit)
        velocity.m_x = 0;
        //velocity.m_x = -velocity.m_x;

    if (z<-limit || z>limit)
        velocity.m_z = 0;
        //velocity.m_z = -velocity.m_z;

    _agent->setVelocity(velocity);
    _agent->setPosition(_agent->getPreviousPos()+velocity*step);

}
