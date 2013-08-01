#include "CrowdEngine.h"


const float CrowdEngine::s_initStride = 1;
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

void CrowdEngine::createRandomFlock(int _rows, int _columns, ngl::Vec2 _position, std::string _flock, float _maxStrength, std::string _army)
{
    if (_rows<=0 || _columns<=0)
    {
        std::cout << "CrowdEngine: ERROR: Impossible to create flock "<<_rows << "x" << _columns << std::endl;
        return;
    }

    if (s_loadedBrains.empty())
    {
        std::cout << "CrowdEngine: ERROR: Impossible to create flock: No brains loaded" << std::endl;
        return;
    }

    Agent* myAgent;
    float startingX = _position.m_x - (_columns-1)/2.0 * s_initStride;
    float startingZ = _position.m_y - (_rows-1)/2.0 * s_initStride;

    ngl::Vec3 agentPosition;
    std::string brain;
    std::set<std::string>::const_iterator firstBrain;
    std::vector<Agent*> flockAgents;

    std::cout << "CrowdEngine: Creating flock " << _flock << " " << _rows << "x"
              << _columns <<" (" << _rows*_columns << " agents)" << std::endl;

    for (int i=0; i<_columns; ++i)
        for (int j=0; j<_rows; ++j)
        {
            agentPosition.m_x = startingX+i*s_initStride;
            agentPosition.m_y = 0;
            agentPosition.m_z = startingZ+j*s_initStride;

            firstBrain = s_loadedBrains.begin();
            std::advance(firstBrain,rand() % s_loadedBrains.size());
            brain = *firstBrain;

            myAgent = new Agent(agentPosition,_flock,brain);
            myAgent->addAttribute("army",_army);
            myAgent->setMaxStrength(_maxStrength);

            m_agents.push_back(myAgent);
            flockAgents.push_back(myAgent);
        }

    m_cellPartition->addAgents(flockAgents);
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
        /**
         * One bug here:
         * memory corruption for double delete ¿?
         * GOT IT
         * When loading agents from more than one file, the vector wasn't cleared,
         * therefore some agents were added twice
         */
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
