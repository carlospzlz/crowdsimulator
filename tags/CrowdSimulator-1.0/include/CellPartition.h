#ifndef CELLPARTITION_H
#define CELLPARTITION_H

/**
 * @file CellPartition.h
 * @brief Abstract class which defines the interface of a cell partition mechanism.
 * This is essential for handling a big amount of agents efficiently
 */

#include "Agent.h"


/**
 * @class CellPartition
 * @brief Abstract class which defines the interface of a cell partition mechanism.
 * This is essential for handling a big amount of agents efficiently
 */
class CellPartition
{

protected:
    /**
     * @brief Size of the cell
     */
    int m_cellSize;

public:
    CellPartition(int _cellSize) : m_cellSize(_cellSize) { }
    virtual ~CellPartition() { }

    int getCellSize() const { return m_cellSize; }

    /**
     * @brief Add _agent to the cell partition
     */
    virtual void addAgent(Agent* _agent) = 0;
    /**
     * @brief Add _agents to the cell partition
     */
    virtual void addAgents(const std::vector<Agent *> &_agents) = 0;
    /**
     * @brief Calculate in which cell each agent is
     */
    virtual void updateCells(const std::vector<Agent*> &_agents) = 0;
    /**
     * @brief Calculate the neighbours of _agents
     */
    virtual void updateNeighbours(const std::vector<Agent *> &_agents) = 0;
    /**
     * @brief Recalculate the complete cell partition. This must be done
     * when the cellSize is changed
     */
    virtual void rearrangePartition(int _cellSize, const std::vector<Agent *> &_agents) = 0;
    /**
     * @brief Print information about the cells
     */
    virtual void printCells() const = 0;
    /**
     * @brief Remove all the agents from the cells
     */
    virtual void clear() = 0;

};

#endif // CELLPARTITION_H
