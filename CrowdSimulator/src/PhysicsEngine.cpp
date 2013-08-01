#include <PhysicsEngine.h>

PhysicsEngine::PhysicsEngine(physicsEngineType _type)
{
    m_type = _type;
    m_boundingBoxSize = 100;
    m_friction = 0.01;
    m_gravity = 0.05;
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

void PhysicsEngine::applyFriction(Agent *_agent)
{
    _agent->applyForce(-m_friction*_agent->getVelocity());
}

void PhysicsEngine::checkCollisionGround(Agent *_agent)
{
    ngl::Vec4 position = _agent->getPosition();

    if ( position.m_y < 0)
    {
        position.m_y = 0;
        _agent->setPosition(position);

        ngl::Vec4 velocity = _agent->getVelocity();
        if (velocity.m_y < 0)
        {
            velocity.m_y = 0;
            _agent->setVelocity(velocity);
        }
    }
}

void PhysicsEngine::applyGravity(Agent *_agent)
{
    _agent->applyForce(ngl::Vec4(0,-m_gravity,0,0));
}
