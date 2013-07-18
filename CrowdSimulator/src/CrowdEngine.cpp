#include "CrowdEngine.h"


const float CrowdEngine::s_initStride = 1;
const int CrowdEngine::s_cellSize = 2;
const float CrowdEngine::s_step = 0.05;
const float CrowdEngine::s_friction = 0.01;

const std::string CrowdEngine::s_brainsPath = "brains/";
std::set<std::string> CrowdEngine::s_loadedBrains;
lua_State* CrowdEngine::s_luaState;


CrowdEngine::CrowdEngine() : m_cellPartition(s_cellSize)
{
    s_luaState = luaL_newstate();
    Agent::setLuaState(s_luaState);
    Agent::setStep(s_step);
    Agent::setFriction(s_friction);

    luaL_openlibs(s_luaState);
}

CrowdEngine::~CrowdEngine()
{
    std::cout << "CrowdEngine: closing lua" << std::endl;
    lua_close(s_luaState);

    std::cout << "CrowdEngine: deleting all agents" << std::endl;
    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        delete (*currentAgent);
    }
}

void CrowdEngine::loadBrain(std::string _brain)
{
    if ( !s_loadedBrains.count(_brain) )
    {
        std::ostringstream path;
        path << s_brainsPath << _brain << ".lua";
        luaL_dofile(s_luaState,path.str().c_str());
        s_loadedBrains.insert(_brain);
        std::cout << "CrowdEngine: Brain " << _brain << " loaded from " << path.str().c_str() << std::endl;
    }
    else
    {
        std::cout << "CrowdEngine: Brain " << _brain << " already loaded" << std::endl;
    }
}

void CrowdEngine::addAgent(Agent* agent)
{
    std::cout << "CrowdEngine: adding agent " << agent->getAgentID() << " to the crowd" << std::endl;

    std::string brain = agent->getBrain();
    if ( s_loadedBrains.count(brain) )
    {
        m_agents.push_back(agent);
        m_cellPartition.addAgent(agent);

    }
    else
    {
        std::cout << "CrowdEngine:  ERROR: Agent " << agent->getAgentID() <<
                     " not added, the brain " << brain << " is not loaded" << std::endl;
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

    m_cellPartition.addAgents(flockAgents);
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

    std::vector<Agent*>::iterator endAgent = m_agents.end();
    for(std::vector<Agent*>::iterator currentAgent = m_agents.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        (*currentAgent)->execute();
    }

}
