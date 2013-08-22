#ifndef SPHEREPE_H
#define SPHEREPE_H

/**
 * @file SpherePE.h
 * @brief Specialized sphere-based PhysicsEngine
 */

#include <PhysicsEngine.h>

/**
 * @class SpherePE
 * @brief Specialized sphere-based PhysicsEngine
 */
class SpherePE : public PhysicsEngine
{

public:
    SpherePE() : PhysicsEngine(PE_SPHERE) { }

    /**
     * @brief Check if there exists collision between two agents and
     * fix their positions and velocities
     */
    void checkCollision(Agent *_a1, Agent *_a2);

};

#endif // SPHEREPE_H
