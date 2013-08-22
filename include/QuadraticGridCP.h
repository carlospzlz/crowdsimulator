#ifndef QUADRATICGRIDCP_H
#define QUADRATICGRIDCP_H

/**
 * @file QuadraticGridCP.h
 * @brief Specialized 2D CellPartition
 */

#include "CellPartition.h"
#include <set>

/**
 * @class QuadraticGridCP
 * @brief Specialized 2D CellPartition
 */
class QuadraticGridCP : public CellPartition
{

private:
    /**
     * @brief Map which store the cell indexes and the agent in each one
     */
    std::map<std::pair<int,int>,std::list<Agent*> > m_cells;
    /**
     * @brief Calculates the index of the corresponding cell from a position
     */
    inline std::pair<int,int> cell(ngl::Vec4 _position) const;
    /**
     * @brief Find the agents in the cells which contains a squared surface
     */
    void findAgentsInCells (ngl::Vec3 _leftDownCornerBox, ngl::Vec3 _rightUpCornerBox, std::vector<Agent*> &_agentsInCells);
    /**
     * @brief Find the neighbours of _agent from the agents in a group of cells
     */
    void findNeighboursInAgents (const Agent* _agent, const std::vector<Agent*> &_agentsInCells, std::vector<Agent*> &_neighbours) const;

public:
    QuadraticGridCP(int _cellSize) : CellPartition(_cellSize) { }
    ~QuadraticGridCP(){ }
    /**
     * @brief Add _agent to the cell partition
     */
    void addAgent(Agent* _agent);
    /**
     * @brief Add _agents to the cell partition
     */
    void addAgents(const std::vector<Agent *> &_agents);
    /**
     * @brief Update location of _agents in the cell partition
     */
    void updateCells(const std::vector<Agent*> &_agents);
    /**
     * @brief Update the neighbours of _agents
     */
    void updateNeighbours(const std::vector<Agent *> &_agents);
    /**
     * @brief Recalculate the complete cell partition. This must be done
     * when the cellSize is changed
     */
    void rearrangePartition(int _cellSize, const std::vector<Agent *> &_agents);
    /**
     * @brief Print information about the cells
     */
    void printCells() const;
    /**
     * @brief Remove all the agents from the cells
     */
    void clear() { m_cells.clear(); }
};

#endif // QUADRATICGRIDCP_H
