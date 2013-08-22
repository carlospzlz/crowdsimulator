#ifndef RADIALPE_H
#define RADIALPE_H

/**
 * @file CylinderPE.h
 * @brief Specialized cylinder-based PhysicsEngine
 */

#include "PhysicsEngine.h"

/**
 * @class CylinderPE
 * @brief Specialized cylinder-based PhysicsEngine
 */
class CylinderPE : public PhysicsEngine
{

public:
    CylinderPE() : PhysicsEngine(PE_CYLINDER) { }

    /**
     * @brief Check if there exists collision between two agents and
     * fix their positions and velocities
     */
    void checkCollision(Agent *_a1, Agent *_a2);

};

#endif // RADIALPE_H
