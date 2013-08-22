#ifndef CROWDENGINE_H
#define CROWDENGINE_H

/**
 * @file CrowdEngine.h
 * @brief This is the core class of the application. It is the abstract virtual world;
 * it stores and handle the agents
 */

#include "CellPartition.h"
#include "PhysicsEngine.h"
#include "ngl/Vec2.h"
#include <math.h>


/**
 * @class CrowdEngine
 * @brief This is the core class of the application. It is the abstract virtual world;
 * it stores the agents and take care of all the updates
 */

class CrowdEngine
{

private:
    /**
     * @brief The path where the brains can be found in the filesystem
     */
    static const std::string s_brainsPath;
    /**
     * @brief A set that takes account about all the brains already loaded
     */
    static std::set<std::string> s_loadedBrains;
    /**
     * @brief A reference to the lua state
     */
    static lua_State* s_luaState;

    /**
     * @brief All the agents in the world
     */
    std::vector<Agent*> m_agents;
    /**
     * @brief A copy of the agents in their initial states needed
     * for the reset of the system
     */
    std::vector<Agent*> m_initialAgents;
    /**
     * @brief A reference to the cell partition in current use
     */
    CellPartition *m_cellPartition;
    /**
     * @brief A reference to the physics engine in current use
     */
    PhysicsEngine *m_physicsEngine;

public:
    CrowdEngine();
    ~CrowdEngine();

    void setCellPartition(CellPartition* _cellPartition) { m_cellPartition = _cellPartition; }
    void setPhysicsEngine(PhysicsEngine* _physicsEngine) { m_physicsEngine = _physicsEngine; }
    void setStep(float _step) { Agent::setStep(_step); }
    void setFriction(float _friction) { m_physicsEngine->setFriction(_friction); }
    void setGravity(float _gravity) { m_physicsEngine->setGravity(_gravity); }

    std::vector<Agent*>::const_iterator getAgentsBegin() { return m_agents.begin(); }
    std::vector<Agent*>::const_iterator getAgentsEnd() { return m_agents.end(); }
    float getBoundingBoxSize() const { return m_physicsEngine->getBoundingBoxSize(); }
    int getCellSize() const { return m_cellPartition->getCellSize(); }
    physicsEngineType getPEType() const { return m_physicsEngine->getType(); }
    int getAgentsNumber() const { return m_agents.size(); }

    /**
     * @brief Brings a brain (lua function) to the memory
     */
    void loadBrain(std::string _brain);
    /**
     * @brief Add _agent to the world
     */
    void addAgent(Agent *_agent);
    /**
     * @brief Add _agents to the world
     */
    void addAgents(const std::vector<Agent *> &_agents);
    /**
     * @brief Print information about the agents in the world
     */
    void printAgents();
    /**
     * @brief Print information about the cell partition
     */
    void printCellPartition() { m_cellPartition->printCells(); }
    /**
     * @brief Reconfigure the cell partition with a different size of cell
     */
    void rearrangePartition(int _cellSize) { m_cellPartition->rearrangePartition(_cellSize, m_agents); }
    /**
     * @brief Update the world
     */
    void update();
    /**
     * @brief Remove all the current agents in the world
     */
    void clear();
    /**
     * @brief Load the initial state of the agents in the world
     */
    void restart();
    /**
     * @brief Multiply the collision radius of all the agents by _scale
     */
    void scaleCollisionRadius(float _scale);
    /**
     * @brief Adjust the size of the bounding box
     */
    void setBoundingBoxSize(float _boundingBoxSize) { m_physicsEngine->setBoundingBoxSize(_boundingBoxSize); }

};

#endif // CROWDENGINE_H
