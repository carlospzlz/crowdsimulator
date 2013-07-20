#ifndef QUADRATICGRIDCP_H
#define QUADRATICGRIDCP_H

#include "CellPartition.h"
#include <set>

class QuadraticGridCP : public CellPartition
{

private:
    std::map<std::pair<int,int>,std::list<Agent*> > m_cells;
    inline std::pair<int,int> cell(ngl::Vec4 _position) const;
    void findAgentsInCells (ngl::Vec3 _leftDownCornerBox, ngl::Vec3 _rightUpCornerBox, std::vector<Agent*> &_agentsInCells);
    void findNeighboursInAgents (const Agent* _agent, const std::vector<Agent*> &_agentsInCells, std::vector<Agent*> &_neighbours) const;

public:
    QuadraticGridCP(int _cellSize) : CellPartition(_cellSize) { }
    ~QuadraticGridCP(){ }
    void addAgent(Agent* _agent);
    void addAgents(const std::vector<Agent *> &_agents);
    void updateCells(const std::vector<Agent*> &_agents);
    void updateNeighbours(const std::vector<Agent *> &_agents);
    void rearrangePartition(int _cellSize, const std::vector<Agent *> &_agents);
    void printCells() const;

};

#endif // QUADRATICGRIDCP_H
