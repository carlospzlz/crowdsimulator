#include "CellPartition.h"

inline std::pair<int,int> CellPartition::cell(ngl::Vec4 _position)
{
    //This is the way of indexing
    return std::pair<int,int>(ceil(_position.m_x/(float)m_cellSize),
                              ceil(_position.m_z/(float)m_cellSize) );
}

void CellPartition::addAgents(std::vector<Agent*> &_agents)
{
    Agent* agent;

    std::vector<Agent*>::iterator endAgent = _agents.end();
    for(std::vector<Agent*>::iterator currentAgent = _agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        m_cells[cell(agent->getPosition())].push_back(agent);
    }
}

void CellPartition::updateCells(std::vector<Agent *> &_agents)
{
    Agent* agent;
    std::pair<int,int> currentCell, previousCell;

    std::vector<Agent*>::iterator endAgent = _agents.end();
    for(std::vector<Agent*>::iterator currentAgent = _agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        currentCell = cell(agent->getPosition());
        previousCell = cell(agent->getPreviousPos());

        if ( currentCell != previousCell )
        {
            m_cells[currentCell].push_back(agent);
            m_cells[previousCell].remove(agent);
        }
    }

}

void CellPartition::printCells()
{
    std::map<std::pair<int,int>,std::list<Agent*> >::iterator endCell = m_cells.end();
    std::list<Agent*>::iterator endAgent;

    std::map<std::pair<int,int>,std::list<Agent*> >::iterator currentCell;
    for(currentCell = m_cells.begin(); currentCell!=endCell; ++currentCell)
    {
        endAgent = currentCell->second.end();
        std::cout << "CELL {" <<currentCell->first.first << ", " << currentCell->first.second
                  << "} (" << currentCell->second.size() << ")" << std::endl;

        for(std::list<Agent*>::iterator currentAgent=currentCell->second.begin(); currentAgent!=endAgent; ++currentAgent)
        {
            std::cout << *currentAgent << "(" << (*currentAgent)->getPosition().m_x << "," << (*currentAgent)->getPosition().m_z << ")\t";
        }
        std::cout << std::endl;
    }
}

void CellPartition::findNeighbours(std::vector<Agent *> &_agents)
{

}
