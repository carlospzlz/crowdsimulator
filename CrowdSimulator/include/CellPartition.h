#ifndef CELLPARTITION_H
#define CELLPARTITION_H

#include "Agent.h"

class CellPartition
{

protected:
    int m_cellSize;

public:
    CellPartition(int _cellSize) : m_cellSize(_cellSize) { }
    virtual ~CellPartition() { }
    int getCellSize() const { return m_cellSize; }
    virtual void addAgent(Agent* _agent) = 0;
    virtual void addAgents(const std::vector<Agent *> &_agents) = 0;
    virtual void updateCells(const std::vector<Agent*> &_agents) = 0;
    virtual void updateNeighbours(const std::vector<Agent *> &_agents) = 0;
    virtual void rearrangePartition(int _cellSize, const std::vector<Agent *> &_agents) = 0;
    virtual void printCells() const = 0;

};

#endif // CELLPARTITION_H
