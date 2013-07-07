#ifndef CELLPARTITION_H
#define CELLPARTITION_H

#include "Agent.h"

class CellPartition
{

private:
    int m_cellSize;
    std::map<std::pair<int,int>,std::list<Agent*> > m_cells;
    inline std::pair<int,int> cell(ngl::Vec4 _position);

public:
    CellPartition(int _cellSize) { m_cellSize = _cellSize; }
    void addAgents(std::vector<Agent *> &_agents);
    void updateCells(std::vector<Agent*> &_agents);
    void printCells();
    void findNeighbours(std::vector<Agent*> &_agents);


};

#endif // CELLPARTITION_H
