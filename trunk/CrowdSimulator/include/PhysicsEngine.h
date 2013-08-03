#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "Agent.h"

enum physicsEngineType
{
    PE_CYLINDER,
    PE_SPHERE
};

class PhysicsEngine
{

private:
    physicsEngineType m_type;
    float m_boundingBoxSize;
    float m_friction;
    float m_gravity;

protected:
    static const float s_epsilon;

public:
    PhysicsEngine(physicsEngineType _type);

    void setBoundingBoxSize(float _boxSize) { m_boundingBoxSize = _boxSize; }
    void setFriction(float _friction) { m_friction = _friction; }
    void setGravity(float _gravity) { m_gravity = _gravity; }

    float getBoundingBoxSize() const { return m_boundingBoxSize; }
    physicsEngineType getType() const { return m_type; }

    virtual void checkCollision(Agent* _a1, Agent* _a2) = 0;

    void checkCollisionBoundingBox(Agent* _agent);
    void checkCollisionGround(Agent* _agent);
    void applyFriction(Agent* _agent);
    void applyGravity(Agent* _agent);

};

#endif // PHYSICSENGINE_H
