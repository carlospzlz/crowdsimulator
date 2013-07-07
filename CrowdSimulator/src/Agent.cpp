#include "Agent.h"

Agent::Agent()
{
    m_active = true;
    m_mass = 1;
    m_maxStrength = 1;
    m_strength = m_maxStrength;
    m_maxSpeed = 3;
    m_transformation.reset();
    m_previousTransform = m_transformation;
    m_flockID = -1;
    m_state = "unknown";
    m_visionRange = 0;
    m_label = "unknown";
    m_primitive = "sphere";
}

Agent::Agent(ngl::Vec3 _pos, int _fid)
{
    m_active = true;
    m_mass = 1;
    m_maxStrength = 1;
    m_strength = m_maxStrength;
    m_maxSpeed = 3;
    m_transformation.reset();
    m_transformation.setPosition(_pos);
    m_previousTransform = m_transformation;
    m_flockID = _fid;
    m_state = "unknown";
    m_visionRange = 0;
    m_label = "unknown";
    m_primitive = "sphere";
}

void Agent::print() const
{
    std::cout << "AGENT " << this << "(Flock " << m_flockID<< ")" << std::endl;
    std::cout << "position = " << m_transformation.getPosition() << std::endl;
    std::cout << "neighbours (" << m_neighbours.size() << "):"<< std::endl;

    Agent* agent;
    std::vector<Agent*>::const_iterator endAgent = m_neighbours.end();
    for(std::vector<Agent*>::const_iterator currentAgent = m_neighbours.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = (*currentAgent);
        std::cout << "[" << agent->getPosition().m_x << ", " << agent->getPosition().m_z << "], ";
    }
    std::cout << std::endl;

    std::cout << std::endl;
}

float Agent::distance(const Agent *_agent) const
{
    return (_agent->getPosition()-m_transformation.getPosition()).length();
}
