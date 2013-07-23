#ifndef CROWDENGINE_H
#define CROWDENGINE_H

#include "CellPartition.h"
#include "ngl/Vec2.h"
#include <math.h>


class CrowdEngine
{

private:
    static const float s_initStride;
    static const std::string s_brainsPath;
    static std::set<std::string> s_loadedBrains;
    static lua_State* s_luaState;

    std::vector<Agent*> m_agents;
    std::vector<Agent*> m_initialAgents;
    CellPartition *m_cellPartition;

public:
    CrowdEngine();
    ~CrowdEngine();

    void setCellPartition(CellPartition* _cellPartition) { m_cellPartition = _cellPartition; }
    void setStep(float _step) { Agent::setStep(_step); }
    void setFriction(float _friction) {Agent::setFriction(_friction); }

    std::vector<Agent*>::const_iterator getAgentsBegin() { return m_agents.begin(); }
    std::vector<Agent*>::const_iterator getAgentsEnd() { return m_agents.end(); }
    int getCellSize() const { return m_cellPartition->getCellSize(); }

    void loadBrain(std::string _brain);
    void addAgent(Agent *_agent);
    void addAgents(const std::vector<Agent *> &_agents);
    void createRandomFlock(int _rows, int _columns, ngl::Vec2 _position, std::string _flock, float _maxStrength, std::string _army);
    void printAgents();
    void printCellPartition() { m_cellPartition->printCells(); }
    void rearrangePartition(int _cellSize) { m_cellPartition->rearrangePartition(_cellSize, m_agents); }
    void update();
    void clear();
    void restart();
    void scaleCollisionRadius(float _scale);

};

#endif // CROWDENGINE_H
