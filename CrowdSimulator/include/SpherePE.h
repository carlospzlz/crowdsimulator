#ifndef SPHEREPE_H
#define SPHEREPE_H

#include <PhysicsEngine.h>

class SpherePE : public PhysicsEngine
{

public:
    SpherePE() : PhysicsEngine(PE_SPHERE) { }
    void checkCollision(Agent *_a1, Agent *_a2);

};

#endif // SPHEREPE_H
