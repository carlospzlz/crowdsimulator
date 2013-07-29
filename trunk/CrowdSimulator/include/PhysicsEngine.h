#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "Agent.h"

class PhysicsEngine
{

private:
    float m_boundingBoxSize;

public:
    PhysicsEngine();
    void setBoundingBoxSize(float _boxSize) { m_boundingBoxSize=_boxSize; }

    float getBoundingBoxSize() const { return m_boundingBoxSize; }

    virtual void checkCollision(Agent* a1, Agent* a2) = 0;
    void checkCollisionBoundingBox(Agent* _agent);

};

#endif // PHYSICSENGINE_H
