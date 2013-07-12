#include "Agent.h"


lua_State* Agent::s_luaState;

Agent::Agent()
{
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
    m_mass = 1;
    m_maxStrength = 1;
    m_visionRadius = 3;
    m_strength = m_maxStrength;
    m_maxSpeed = 3;
    m_transformation.reset();
    m_transformation.setPosition(_pos);
    m_previousTransform = m_transformation;
    m_messages["flock"] = _flock;
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

    std::cout << "messages (" << m_messages.size() << "):" << std::endl;
    std::pair<std::string,std::string> message;
    std::map<std::string,std::string>::const_iterator endMessage = m_messages.end();
    for(std::map<std::string,std::string>::const_iterator currentMessage = m_messages.begin();
        currentMessage!=endMessage; ++currentMessage)
    {
        message = (*currentMessage);
        std::cout << "[" << message.first << ", " << message.second << "], ";
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
    // This is the funny part, playing with lua

    // Push all the data on the stack
    /**
     * function: global
     * m_strength: number
     * m_position: table
     * m_velocity: table
     * m_state: string
     * m_messages: table
     * m_neighbours: table
     *          > m_mass
     *          > m_strength
     *          > m_position
     *          > m_maxSpeed
     *          > m_state
     *          > m_messages
     * m_faces: table?
     */

    std::cout << "Agent " << this << ": Executing" << std::endl;

    // Stack[1]: global function name
    lua_getglobal(s_luaState,m_brain.c_str());

    // Stack[2]: m_strength
    lua_pushnumber(s_luaState,m_strength);

    // Stack[3]: position
    ngl::Vec4 position = m_transformation.getPosition();
    lua_createtable(s_luaState,3,0);

    lua_pushstring(s_luaState,"m_x");
    lua_pushnumber(s_luaState,position.m_x);
    lua_settable(s_luaState,3);
    lua_pushstring(s_luaState,"m_y");
    lua_pushnumber(s_luaState,position.m_y);
    lua_settable(s_luaState,3);
    lua_pushstring(s_luaState,"m_z");
    lua_pushnumber(s_luaState,position.m_z);
    lua_settable(s_luaState,3);

    // Stack[4]: m_velocity
    lua_createtable(s_luaState,3,0);

    lua_pushstring(s_luaState,"m_x");
    lua_pushnumber(s_luaState,m_velocity.m_x);
    lua_settable(s_luaState,4);
    lua_pushstring(s_luaState,"m_y");
    lua_pushnumber(s_luaState,m_velocity.m_y);
    lua_settable(s_luaState,4);
    lua_pushstring(s_luaState,"m_z");
    lua_pushnumber(s_luaState,m_velocity.m_z);
    lua_settable(s_luaState,4);

    // Stack[5]: m_state
    lua_pushstring(s_luaState,m_state.c_str());

    // Stack[6]: messages
    // Do not need to check the stack, just needed 2 temporal slots
    std::pair<std::string,std::string> message;
    int nMessages = m_messages.size();
    lua_createtable(s_luaState,nMessages,0);

    std::map<std::string,std::string>::const_iterator endMessage = m_messages.end();
    for(std::map<std::string,std::string>::const_iterator currentMessage = m_messages.begin();
        currentMessage!=endMessage; ++currentMessage)
    {
        message = (*currentMessage);
        lua_pushstring(s_luaState, message.first.c_str());
        lua_pushstring(s_luaState, message.second.c_str());
        lua_settable(s_luaState, 6);
    }

    // Stack[7]: neighbours
    Agent* agent;
    int index = 1;
    int nNeighbours = m_neighbours.size();
    lua_createtable(s_luaState,nNeighbours,0);

    std::vector<Agent*>::const_iterator endAgent = m_neighbours.end();
    for(std::vector<Agent*>::const_iterator currentAgent = m_neighbours.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        /**
         * Each agent is gonna be a table stored temporaly at Stack[8],
         * but then it will be added to the table of neightbours at Stack[7]
         */

        agent = (*currentAgent);
        lua_createtable(s_luaState,2,0);

        // mass
        lua_pushstring(s_luaState,"m_mass");
        lua_pushnumber(s_luaState,agent->getMass());
        lua_settable(s_luaState,8);

        // strength
        lua_pushstring(s_luaState,"m_strength");
        lua_pushnumber(s_luaState,agent->getStrength());
        lua_settable(s_luaState,8);

        // position
        position = agent->getPosition();
        // m_position key at Stack[9]
        lua_pushstring(s_luaState,"m_position");
        // Position table at Stack[10]
        lua_createtable(s_luaState,3,0);
        lua_pushstring(s_luaState,"m_x");
        lua_pushnumber(s_luaState,position.m_x);
        lua_settable(s_luaState,10);
        lua_pushstring(s_luaState,"m_y");
        lua_pushnumber(s_luaState,position.m_y);
        lua_settable(s_luaState,10);
        lua_pushstring(s_luaState,"m_z");
        lua_pushnumber(s_luaState,position.m_z);
        lua_settable(s_luaState,10);
        // Add position table to the Agent-table
        lua_settable(s_luaState,8);


        // state
        lua_pushstring(s_luaState,"m_state");
        lua_pushstring(s_luaState,agent->getState().c_str());
        lua_settable(s_luaState,8);


        // Messages
        nMessages = m_messages.size();
        // m_messages key at Stack[9]
        lua_pushstring(s_luaState,"m_messages");
        // Messages table at Stack[10]
        lua_createtable(s_luaState,nMessages,0);

        endMessage = m_messages.end();
        for(std::map<std::string,std::string>::const_iterator currentMessage = m_messages.begin();
            currentMessage!=endMessage; ++currentMessage)
        {
            message = (*currentMessage);
            lua_pushstring(s_luaState, message.first.c_str());
            lua_pushstring(s_luaState, message.second.c_str());
            lua_settable(s_luaState, 10);
        }
        // Add message table to the Agent-table
        lua_settable(s_luaState,8);

        lua_rawseti(s_luaState,7,index);
        ++index;
    }


    // Take a lot of care here (func+6)
    lua_call(s_luaState,6,0);

    //m_transformation.setPosition(ngl::Vec3((int)lua_tointeger(s_luaState,-1),0,0));

    // Leave the lua stack as you found it! (empty)
    //lua_pop(s_luaState,2);


}
