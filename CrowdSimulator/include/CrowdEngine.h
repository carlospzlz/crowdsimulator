#ifndef CROWDENGINE_H
#define CROWDENGINE_H

#include "CellPartition.h"
#include "ngl/Vec2.h"
#include <math.h>

class CrowdEngine
{

private:
    static const float s_initStride;
    static const int s_cellSize;
    static const float s_neighbourhoodRadius;
    int m_numberOfFlocks;
    std::vector<Agent*> m_agents;

    CellPartition m_cellPartition;

public:
    CrowdEngine();
    ~CrowdEngine();
    std::vector<Agent*>::const_iterator getAgentsBegin() { return m_agents.begin(); }
    std::vector<Agent*>::const_iterator getAgentsEnd() { return m_agents.end(); }
    int getCellSize() const { return s_cellSize; }
    float getRadius() const { return s_neighbourhoodRadius; }

    void createFlock(int _rows, int _columns, ngl::Vec2 _position);
    void printAgents();
    void printCellPartition() { m_cellPartition.printCells(); }
    void update();

};

#endif // CROWDENGINE_H
