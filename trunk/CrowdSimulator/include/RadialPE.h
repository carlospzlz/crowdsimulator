#ifndef RADIALPE_H
#define RADIALPE_H

#include "PhysicsEngine.h"

class RadialPE : public PhysicsEngine
{

public:
    void checkCollision(Agent *a1, Agent *a2);

};

#endif // RADIALPE_H
