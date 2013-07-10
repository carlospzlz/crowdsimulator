#include "CrowdEngine.h"

const float CrowdEngine::s_initStride = 2;

const int CrowdEngine::s_cellSize = 4;

const float CrowdEngine::s_neighbourhoodRadius = 3;

CrowdEngine::CrowdEngine() : m_cellPartition(s_cellSize)
{
    m_numberOfFlocks = 0;
}

CrowdEngine::~CrowdEngine()
{
    std::cout << "CrowdEngine: deleting all agents" << std::endl;

    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        delete (*currentAgent);
    }
}

void CrowdEngine::createFlock(int _rows, int _columns, ngl::Vec2 _position)
{
    if (_rows<=0 || _columns<=0)
    {
        std::cout << "CrowdEngine: ERROR: Impossible to create flock "<<_rows << "x" << _columns << std::endl;
        return;
    }

    Agent* myAgent;
    float startingX = _position.m_x - (_columns-1)/2.0 * s_initStride;
    float startingZ = _position.m_y - (_rows-1)/2.0 * s_initStride;

    ngl::Vec3 agentPosition;
    std::vector<Agent*> flockAgents;

    std::cout << "CrowdEngine: Creating flock " << _rows << "x"<< _columns
              <<" (" << _rows*_columns << " agents)" << std::endl;

    for (int i=0; i<_columns; ++i)
        for (int j=0; j<_rows; ++j)
        {
            agentPosition.m_x = startingX+i*s_initStride;
            agentPosition.m_y = 0;
            agentPosition.m_z = startingZ+j*s_initStride;

            myAgent = new Agent(agentPosition,m_numberOfFlocks);

            m_agents.push_back(myAgent);
            flockAgents.push_back(myAgent);
        }

    m_cellPartition.addAgents(flockAgents);
    ++m_numberOfFlocks;
}

void CrowdEngine::printAgents()
{
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        (*currentAgent)->print();
    }

}

void CrowdEngine::update()
{
    m_cellPartition.updateCells(m_agents);

    m_cellPartition.updateNeighbours(m_agents);

}
