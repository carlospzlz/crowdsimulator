#include "CrowdEngine.h"

CrowdEngine::CrowdEngine() : m_cellPartition(m_cellSize)
{
    m_numberOfFlocks = 0;
}

CrowdEngine::~CrowdEngine()
{
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        delete (*currentAgent);
    }
    std::cout << "CrowdEngine: All agents deleted" << std::endl;
}

void CrowdEngine::createFlock(int _rows, int _columns, ngl::Vec2 _position)
{
    Agent* myAgent;
    float startingX = _position.m_x - (_columns-1)/2.0 * m_initStride;
    float startingZ = _position.m_y - (_rows-1)/2.0 * m_initStride;

    ngl::Vec3 agentPosition;
    std::vector<Agent*> flockAgents;

    for (int i=0; i<_columns; ++i)
        for (int j=0; j<_rows; ++j)
        {
            agentPosition.m_x = startingX+i*m_initStride;
            agentPosition.m_y = 0;
            agentPosition.m_z = startingZ+j*m_initStride;

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
    m_cellPartition.printCells();
}
