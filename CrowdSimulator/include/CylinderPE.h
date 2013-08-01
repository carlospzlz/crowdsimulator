#ifndef RADIALPE_H
#define RADIALPE_H

#include "PhysicsEngine.h"

class CylinderPE : public PhysicsEngine
{

public:
    CylinderPE() : PhysicsEngine(PE_CYLINDER) { }
    void checkCollision(Agent *_a1, Agent *_a2);

};

#endif // RADIALPE_H
