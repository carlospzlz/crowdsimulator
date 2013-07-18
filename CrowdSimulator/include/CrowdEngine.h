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
    static const float s_step;
    static const float s_friction;
    static const std::string s_brainsPath;
    static std::set<std::string> s_loadedBrains;
    static lua_State* s_luaState;

    std::vector<Agent*> m_agents;
    CellPartition m_cellPartition;

public:
    CrowdEngine();
    ~CrowdEngine();
    std::vector<Agent*>::const_iterator getAgentsBegin() { return m_agents.begin(); }
    std::vector<Agent*>::const_iterator getAgentsEnd() { return m_agents.end(); }
    int getCellSize() const { return s_cellSize; }

    void loadBrain(std::string _brain);
    void addAgent(Agent *agent);
    void createRandomFlock(int _rows, int _columns, ngl::Vec2 _position, std::string _flock, float _maxStrength, std::string _army);
    void loadCrowd(std::string _fileName);
    void printAgents();
    void printCellPartition() { m_cellPartition.printCells(); }
    void update();

};

#endif // CROWDENGINE_H
