#include "Agent.h"


lua_State* Agent::s_luaState;

Agent::Agent()
{
    m_active = true;
    m_mass = 1;
    m_maxStrength = 1;
    m_visionRadius = 3;
    m_strength = m_maxStrength;
    m_maxSpeed = 3;
    m_transformation.reset();
    m_previousTransform = m_transformation;
    m_state = "unknown";
    m_brain = "no brain";
    m_visionRange = 0;
    m_primitive = "sphere";
}

Agent::Agent(ngl::Vec3 _pos, std::string _flock, std::string _brain)
{
    m_active = true;
    m_mass = 1;
    m_maxStrength = 1;
    m_visionRadius = 3;
    m_strength = m_maxStrength;
    m_maxSpeed = 3;
    m_transformation.reset();
    m_transformation.setPosition(_pos);
    m_previousTransform = m_transformation;
    m_labels["flock"] = _flock;
    m_state = "unknown";
    m_brain = _brain;
    m_visionRange = 0;
    m_primitive = "sphere";
}

void Agent::print() const
{
    std::cout << "AGENT " << this << std::endl;
    std::cout << "position = " << m_transformation.getPosition() << std::endl;
    std::cout << "vision radius = " << m_visionRadius << std::endl;
    std::cout << "brain: " << m_brain << std::endl;

    std::cout << "neighbours (" << m_neighbours.size() << "):"<< std::endl;
    Agent* agent;
    std::vector<Agent*>::const_iterator endAgent = m_neighbours.end();
    for(std::vector<Agent*>::const_iterator currentAgent = m_neighbours.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = (*currentAgent);
        std::cout << agent << "[" << agent->getPosition().m_x << ", " << agent->getPosition().m_z << "], ";
    }
    std::cout << std::endl;

    std::cout << "labels (" << m_labels.size() << "):" << std::endl;
    std::pair<std::string,std::string> label;
    std::map<std::string,std::string>::const_iterator endLabel = m_labels.end();
    for(std::map<std::string,std::string>::const_iterator currentLabel = m_labels.begin();
        currentLabel!=endLabel; ++currentLabel)
    {
        label = (*currentLabel);
        std::cout << "[" << label.first << ", " << label.second << "], ";
    }
    std::cout << std::endl;

    std::cout << std::endl;
}

float Agent::distance(const Agent *_agent) const
{
    return (_agent->getPosition()-m_transformation.getPosition()).length();
}

void Agent::execute()
{
    //it pushes the value of the function on the stack
    std::ostringstream function;
    function << m_brain;
    std::cout << function.str() << std::endl;
    lua_getglobal(s_luaState,function.str().c_str());

    lua_pushnumber(s_luaState,m_transformation.getPosition().m_x);

    lua_call(s_luaState,1,1);

    m_transformation.setPosition(ngl::Vec3((int)lua_tointeger(s_luaState,-1),0,0));

    lua_pop(s_luaState,1);

}
