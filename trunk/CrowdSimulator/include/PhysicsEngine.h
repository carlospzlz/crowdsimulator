#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "Agent.h"

class PhysicsEngine
{

public:
    virtual void checkCollision(Agent* a1, Agent* a2) = 0;

};

#endif // PHYSICSENGINE_H
