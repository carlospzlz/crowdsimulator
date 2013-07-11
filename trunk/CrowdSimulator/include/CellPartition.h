#ifndef CELLPARTITION_H
#define CELLPARTITION_H

#include "Agent.h"
#include <set>

class CellPartition
{

private:
    const int m_cellSize;
    std::map<std::pair<int,int>,std::list<Agent*> > m_cells;
    inline std::pair<int,int> cell(ngl::Vec4 _position) const;
    void findAgentsInCells (ngl::Vec3 _leftDownCornerBox, ngl::Vec3 _rightUpCornerBox, std::vector<Agent*> &_agentsInCells);
    void findNeighboursInAgents (const Agent* _agent, const std::vector<Agent*> &_agentsInCells, std::vector<Agent*> &_neighbours) const;

public:
    CellPartition(int _cellSize) : m_cellSize(_cellSize) { }
    void addAgent(Agent* _agent);
    void addAgents(const std::vector<Agent *> &_agents);
    void updateCells(const std::vector<Agent*> &_agents);
    void printCells() const;
    void updateNeighbours(const std::vector<Agent *> &_agents);

};

#endif // CELLPARTITION_H
