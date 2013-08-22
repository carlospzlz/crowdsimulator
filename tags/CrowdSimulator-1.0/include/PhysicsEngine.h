#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

/**
 * @file PhysicsEngine.h
 * @brief This abstract class represents the interface of a physics engine,
 * which is the responsible of handle the physics of the world, such as
 * external forces or collisions
 */

#include "Agent.h"

/**
 * @enum physicsEngineType
 * @brief The GLWindow needs to know the type of the physics engine
 * in order to draw the appropriate bounding geometry
 */
enum physicsEngineType
{
    PE_CYLINDER,
    PE_SPHERE
};

/**
 * @class PhysicsEngine
 * @brief This abstract class represents the interface of a physics engine,
 * which is the responsible of handle the physics of the world, such as
 * external forces or collisions
 */
class PhysicsEngine
{

private:
    /**
     * @brief Type of the physics engine
     */
    physicsEngineType m_type;
    /**
     * @brief Size of the box which wraps the world
     */
    float m_boundingBoxSize;
    /**
     * @brief Magnitude of the friction force
     */
    float m_friction;
    /**
     * @brief Magnitude of the gravity force
     */
    float m_gravity;

protected:
    /**
     * @brief Value used in the comparison of floats
     */
    static const float s_epsilon;

public:
    PhysicsEngine(physicsEngineType _type);

    void setBoundingBoxSize(float _boxSize) { m_boundingBoxSize = _boxSize; }
    void setFriction(float _friction) { m_friction = _friction; }
    void setGravity(float _gravity) { m_gravity = _gravity; }

    float getBoundingBoxSize() const { return m_boundingBoxSize; }
    physicsEngineType getType() const { return m_type; }

    /**
     * @brief Check if there exists collision between two agents and
     * fix their positions and velocities
     */
    virtual void checkCollision(Agent* _a1, Agent* _a2) = 0;

    /**
     * @brief Check collision with box that wraps the world
     */
    void checkCollisionBoundingBox(Agent* _agent);
    /**
     * @brief Check collision with the ground
     */
    void checkCollisionGround(Agent* _agent);
    /**
     * @brief Apply friction force on _agent
     */
    void applyFriction(Agent* _agent);
    /**
     * @brief Apply gravity force on _agent
     */
    void applyGravity(Agent* _agent);

};

#endif // PHYSICSENGINE_H
