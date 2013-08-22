#include "CrowdEngine.h"


std::set<std::string> CrowdEngine::s_loadedBrains;
lua_State* CrowdEngine::s_luaState;


CrowdEngine::CrowdEngine()
{
    s_luaState = luaL_newstate();
    Agent::setLuaState(s_luaState);
    luaL_openlibs(s_luaState);

    Agent::setStep(0.05);
}

CrowdEngine::~CrowdEngine()
{
    std::cout << "CrowdEngine: Closing lua" << std::endl;
    lua_close(s_luaState);

    std::cout << "CrowdEngine: Deleting all agents" << std::endl;
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        delete (*currentAgent);
    }
}

void CrowdEngine::loadBrain(std::string _brain)
{

    unsigned brainNameStartingPos = _brain.find_last_of("/\\")+1;
    unsigned brainNameLength = (_brain.find_last_of(".")) - brainNameStartingPos;
    std::string brainName = _brain.substr(brainNameStartingPos,brainNameLength);

    if (!luaL_dofile(s_luaState,_brain.c_str()))
    {
        s_loadedBrains.insert(brainName);
        std::cout << "CrowdEngine: Brain " << brainName << " loaded from " << _brain << std::endl;
    }
    else
    {
        std::cout << "CrowdEngine: ERROR: " << brainName << " cannot be loaded from " << _brain << std::endl;
    }

}

void CrowdEngine::addAgent(Agent* agent)
{
    std::string brain = agent->getBrain();
    if ( s_loadedBrains.count(brain) )
    {
        m_agents.push_back(agent);
        Agent* initialAgent = new Agent(*agent);
        m_initialAgents.push_back(initialAgent);
        m_cellPartition->addAgent(agent);
    }
    else
    {
        std::cout << "CrowdEngine:  ERROR: Agent " << agent->getAgentID() <<
                     " not added, the brain " << brain << " is not loaded" << std::endl;
    }
}

void CrowdEngine::addAgents(const std::vector<Agent*> &_agents)
{
    std::vector<Agent*>::const_iterator endAgent = _agents.end();
    std::vector<Agent*>::const_iterator currentAgent;
    for (currentAgent = _agents.begin(); currentAgent != endAgent; ++currentAgent)
    {
        addAgent(*currentAgent);
    }
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
    m_cellPartition->updateCells(m_agents);
    m_cellPartition->updateNeighbours(m_agents);

    std::vector<Agent*>::iterator endAgent = m_agents.end();
    std::vector<Agent*>::iterator currentAgent;

    // EXTERNAL FORCES AND EXECUTION
    for(currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        m_physicsEngine->applyGravity(*currentAgent);
        m_physicsEngine->applyFriction(*currentAgent);

        (*currentAgent)->execute();
    }

    // COLLISIONS WITH OTHER AGENTS
    std::set<int> collisionPair;
    std::set<std::set<int> > checkedAgents;
    std::vector<Agent*> neighbours;
    std::vector<Agent*>::iterator endNeighbour;
    std::vector<Agent*>::iterator currentNeighbour;
    for(currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        neighbours = (*currentAgent)->getNeighbours();
        endNeighbour = neighbours.end();
        for(currentNeighbour = neighbours.begin(); currentNeighbour!=endNeighbour; ++currentNeighbour)
        {
            collisionPair.insert((*currentAgent)->getAgentID());
            collisionPair.insert((*currentNeighbour)->getAgentID());
            if (checkedAgents.count(collisionPair)!=1)
            {
                //check collision
                m_physicsEngine->checkCollision(*currentAgent,*currentNeighbour);
                checkedAgents.insert(collisionPair);
            }
            collisionPair.clear();
        }
    }

    //COLLISIONS WITH GROUND AND BOUNDING BOX (THESE RULE OVER THE PREVIOUS ONES)
    for(currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        m_physicsEngine->checkCollisionGround(*currentAgent);
        m_physicsEngine->checkCollisionBoundingBox(*currentAgent);
    }

}

void CrowdEngine::clear()
{
    m_agents.clear();
    m_initialAgents.clear();
    m_cellPartition->clear();
}

void CrowdEngine::restart()
{
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    std::vector<Agent*>::iterator currentAgent;
    for(currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        //clear the vector...
        delete *currentAgent;
    }
    m_agents.clear();

    std::vector<Agent*>::const_iterator endInitialAgent = m_initialAgents.end();
    std::vector<Agent*>::const_iterator currentInitialAgent;
    for( currentInitialAgent = m_initialAgents.begin(); currentInitialAgent!=endInitialAgent; ++currentInitialAgent)
    {
        m_agents.push_back(new Agent(**currentInitialAgent));
    }
    m_cellPartition->clear();
    m_cellPartition->addAgents(m_agents);
}

void CrowdEngine::scaleCollisionRadius(float _scale)
{
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    std::vector<Agent*>::iterator currentAgent;
    for(currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        (*currentAgent)->scaleCollisionRadius(_scale);
    }

    endAgent = m_initialAgents.end();
    for(currentAgent = m_initialAgents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        (*currentAgent)->scaleCollisionRadius(_scale);
    }

}
