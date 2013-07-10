#include "CellPartition.h"

inline std::pair<int,int> CellPartition::cell(ngl::Vec4 _position) const
{
    /**
     *This is the way of indexing
     * (i-1)k < n < ik => n belongs to k
     */
    return std::pair<int,int>(ceil(_position.m_x/(float)m_cellSize),
                              ceil(_position.m_z/(float)m_cellSize) );
}

void CellPartition::addAgents(const std::vector<Agent *> &_agents)
{
    Agent* agent;

    std::vector<Agent*>::const_iterator endAgent = _agents.end();
    for(std::vector<Agent*>::const_iterator currentAgent = _agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;
        m_cells[cell(agent->getPosition())].push_back(agent);
    }
}

void CellPartition::updateCells(const std::vector<Agent *> &_agents)
{
    Agent* agent;
    std::pair<int,int> currentCell, previousCell;

    std::cout << "CellPartition: updating cells" << std::endl;

    std::vector<Agent*>::const_iterator endAgent = _agents.end();
    for(std::vector<Agent*>::const_iterator currentAgent = _agents.begin(); currentAgent!=endAgent; ++currentAgent)
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

void CellPartition::printCells() const
{
    std::map<std::pair<int,int>,std::list<Agent*> >::const_iterator endCell = m_cells.end();
    std::list<Agent*>::const_iterator endAgent;

    std::map<std::pair<int,int>,std::list<Agent*> >::const_iterator currentCell;
    for(currentCell = m_cells.begin(); currentCell!=endCell; ++currentCell)
    {
        endAgent = currentCell->second.end();
        std::cout << "CELL {" <<currentCell->first.first << ", " << currentCell->first.second
                  << "} (" << currentCell->second.size() << ")" << std::endl;

        for(std::list<Agent*>::const_iterator currentAgent=currentCell->second.begin(); currentAgent!=endAgent; ++currentAgent)
        {
            std::cout << "(" << (*currentAgent)->getPosition().m_x << "," << (*currentAgent)->getPosition().m_z << ")\t";
        }
        std::cout << std::endl;
    }
}

void CellPartition::updateNeighbours(const std::vector<Agent *> &_agents)
{
    std::set<Agent*> checkedAgents;

    ngl::Vec3 leftDownCornerBox, rightUpCornerBox;
    std::pair<int,int> xLimits, zLimits;
    std::vector<Agent*> agentsInCells, neighbours1, neighbours2;

    Agent *agent1, *agent2;
    float visionRadius;
    std::vector<Agent*>::const_iterator endAgent1, endAgent2;

    endAgent1 = _agents.end();

    std::cout << "CellPartition: updating neighbours" << std::endl;

    for(std::vector<Agent*>::const_iterator currentAgent1 = _agents.begin(); currentAgent1!=endAgent1; ++currentAgent1)
    {
        agent1 = *currentAgent1;
        if ( !checkedAgents.count(agent1) )
        {
            visionRadius = agent1->getVisionRadius();
            leftDownCornerBox.m_x = agent1->getPosition().m_x - visionRadius;
            leftDownCornerBox.m_z = agent1->getPosition().m_z - visionRadius;
            rightUpCornerBox.m_x = agent1->getPosition().m_x + visionRadius;
            rightUpCornerBox.m_z = agent1->getPosition().m_z + visionRadius;

            findAgentsInCells(leftDownCornerBox, rightUpCornerBox, agentsInCells);
            findNeighboursInAgents(agent1, agentsInCells, neighbours1);
            agent1->setNeighbours(neighbours1);

            // FLAG for agent1
            //std::cout << agent1 << std::endl;

            checkedAgents.insert(agent1);

            xLimits.first = floor(leftDownCornerBox.m_x / (float) m_cellSize) * m_cellSize;
            xLimits.second = ceil(rightUpCornerBox.m_x / (float) m_cellSize) * m_cellSize;
            zLimits.first = floor(leftDownCornerBox.m_z / (float) m_cellSize) * m_cellSize;
            zLimits.second = ceil(rightUpCornerBox.m_z / (float) m_cellSize) * m_cellSize;

            endAgent2 = neighbours1.end();
            for(std::vector<Agent*>::const_iterator currentAgent2 = neighbours1.begin();
                currentAgent2!=endAgent2; ++currentAgent2)
            {
                agent2 = *currentAgent2;
                if ( !checkedAgents.count(agent2) )
                {
                    visionRadius = agent2->getVisionRadius();
                    leftDownCornerBox.m_x = agent2->getPosition().m_x - visionRadius;
                    leftDownCornerBox.m_z = agent2->getPosition().m_z - visionRadius;
                    rightUpCornerBox.m_x = agent2->getPosition().m_x + visionRadius;
                    rightUpCornerBox.m_z = agent2->getPosition().m_z + visionRadius;

                    if ( leftDownCornerBox.m_x>xLimits.first && rightUpCornerBox.m_x<= xLimits.second &&
                         leftDownCornerBox.m_z>zLimits.first && rightUpCornerBox.m_z<= zLimits.second)
                    {
                        //The circunference is inside the same group of cells
                        findNeighboursInAgents(agent2, agentsInCells, neighbours2);
                        agent2->setNeighbours(neighbours2);

                        //FLAG for agent2
                        //std::cout << "\t" << agent2 << std::endl;

                        neighbours2.clear();
                        checkedAgents.insert(agent2);
                    }
                }
            }
            neighbours1.clear();
            agentsInCells.clear();
        }
    }

}

void CellPartition::findAgentsInCells(ngl::Vec3 _leftDownCornerBox, ngl::Vec3 _rightUpCornerBox,
                                      std::vector<Agent*> &_agentsInCells)
{
    std::pair<int,int> leftDownCell, rightUpCell, currentCell;
    std::list<Agent*> *pAgentsInCell;

    leftDownCell = cell(_leftDownCornerBox);
    rightUpCell = cell(_rightUpCornerBox);

    for(int x = leftDownCell.first; x <= rightUpCell.first; ++x)
        for(int z = leftDownCell.second; z<= rightUpCell.second; ++z)
        {
            currentCell.first = x;
            currentCell.second = z;
            //do not make the method constant, if so this is not allowed
            pAgentsInCell = &(m_cells[currentCell]);
            _agentsInCells.insert(_agentsInCells.end(), pAgentsInCell->begin(), pAgentsInCell->end());
        }
}

void CellPartition::findNeighboursInAgents(const Agent* _agent, const std::vector<Agent*> &_agentsInCells,
                                           std::vector<Agent*> &_neighbours) const
{
    Agent* agent;

    std::vector<Agent*>::const_iterator endAgent = _agentsInCells.end();
    for(std::vector<Agent*>::const_iterator currentAgent = _agentsInCells.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = *currentAgent;

        if ( agent != _agent && agent->distance(_agent) < agent->getVisionRadius() )
            _neighbours.push_back(agent);
    }

}
