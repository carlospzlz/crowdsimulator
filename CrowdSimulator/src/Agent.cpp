#include "Agent.h"


lua_State* Agent::s_luaState;

float Agent::s_step = 0.05;

int Agent::s_numberOfAgents = 0;

Agent::Agent()
{
    m_agentID = s_numberOfAgents++;
    m_mass = 1;
    m_collisionRadius = m_mass;
    m_maxStrength = 3;
    m_strength = m_maxStrength;
    m_visionRadius = 5;  
    m_maxSpeed = 2;
    m_transformation.reset();
    m_transformation.setScale(m_mass,m_mass,m_mass);
    m_previousTransform = m_transformation;
    m_state = "unknown";
    m_brain = "no brain";
    m_dummy = NULL;
}

Agent::Agent(ngl::Vec4 _pos, std::string _flock, std::string _brain)
{
    m_agentID = s_numberOfAgents++;
    m_mass = 1;
    m_collisionRadius = m_mass;
    m_maxStrength = 3;
    m_strength = m_maxStrength;
    m_visionRadius = 5;
    m_maxSpeed = 2;
    m_transformation.setScale(m_mass,m_mass,m_mass);
    m_transformation.setPosition(_pos);
    m_previousTransform = m_transformation;
    m_attributes["flock"] = _flock;
    m_state = "unknown";
    m_brain = _brain;
    m_dummy = NULL;
}

Agent::Agent(const Agent &_agent)
{
    m_agentID = _agent.getAgentID();
    m_mass = _agent.getMass();
    m_maxStrength = _agent.getMaxStrength();
    m_strength = m_maxStrength;
    m_visionRadius = _agent.getVisionRadius();
    m_maxSpeed = _agent.getMaxSpeed();
    m_transformation.setPosition(_agent.getPosition());
    m_transformation.setScale(m_mass,m_mass,m_mass);
    m_transformation.setRotation(_agent.getTransform().getRotation());
    m_previousTransform = m_transformation;
    m_state = _agent.getState();
    m_brain = _agent.getBrain();
    m_attributes = _agent.getAttributes();
    m_collisionRadius = _agent.getCollisionRadius();
    m_heading = _agent.getHeading();
    m_dummy = _agent.getDummy();
}

void Agent::setMass(float _mass)
{
    m_mass = _mass;
    m_transformation.setScale(_mass,_mass,_mass);
    m_collisionRadius = m_mass;
}

void Agent::print() const
{
    std::cout << "AGENT " << m_agentID << std::endl;
    std::cout << "position = " << m_transformation.getPosition() << std::endl;
    std::cout << "mass = " << m_mass << std::endl;
    std::cout << "strength = " << m_strength << "/" << m_maxStrength << std::endl;
    std::cout << "velocity =" << m_velocity << "("<< m_velocity.length() << ")" << std::endl;
    std::cout << "maxSpeed = " << m_maxSpeed << std::endl;
    std::cout << "vision radius = " << m_visionRadius << std::endl;
    std::cout << "brain: " << m_brain << std::endl;
    std::cout << "state: " << m_state << std::endl;
    std::cout << "heading: " << m_heading << std::endl;

    // attributes
    std::cout << "attributes (" << m_attributes.size() << "):" << std::endl;
    std::pair<std::string,std::string> attribute;
    std::map<std::string,std::string>::const_iterator endAttribute = m_attributes.end();
    for(std::map<std::string,std::string>::const_iterator currentAttribute = m_attributes.begin();
        currentAttribute!=endAttribute; ++currentAttribute)
    {
        attribute = (*currentAttribute);
        std::cout << "{" << attribute.first << ", " << attribute.second << "}, ";
    }
    if (m_attributes.size()>0)
        std::cout << std::endl;

    // inbox
    std::cout << "inbox (" << m_inbox.size() << "):" << std::endl;
    message msge;
    std::vector<message>::const_iterator endMsge = m_inbox.end();
    for(std::vector<message>::const_iterator currentMsge = m_inbox.begin();
        currentMsge!=endMsge; ++currentMsge)
    {
        msge = (*currentMsge);
        std::cout << "{" << msge.agentID << ", " << msge.label << ", "
                  << msge.position << ", " << msge.strength << "}" << std::endl;
    }

    // neighbours
    std::cout << "neighbours (" << m_neighbours.size() << "):"<< std::endl;
    Agent* agent;
    std::vector<Agent*>::const_iterator endAgent = m_neighbours.end();
    for(std::vector<Agent*>::const_iterator currentAgent = m_neighbours.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        agent = (*currentAgent);
        std::cout << agent->getAgentID() << "[" << agent->getPosition().m_x << ", " << agent->getPosition().m_z << "], ";
    }
    if (m_neighbours.size()>0)
        std::cout << std::endl;

    std::cout << std::endl;
}

float Agent::distance(const Agent *_agent) const
{
    return (_agent->getPosition()-m_transformation.getPosition()).length();
}

void Agent::setHeading(ngl::Vec4 _heading)
{
    if (_heading.length()>0)
    {
        m_heading = _heading;
        ngl::Vec4 rotation;

        if (m_heading.m_x != 0)
            rotation.m_y = atan2(-m_heading.m_z,m_heading.m_x) * 180/M_PI;
        else
        {
            if (m_heading.m_z<0)
                rotation.m_y = 90;
            else
                rotation.m_y = -90;
        }

        m_transformation.setRotation(rotation);
    }
}

void Agent::execute()
{
    // This is the funny part, playing with lua

    // Push all the data on the stack
    /**
     * function: global
     * m_agentID: number
     * m_position: table
     * m_strength: number
     * m_maxStrength: number
     * m_velocity: table
     * m_heading: table
     * m_state: string
     * m_attributes: table
     * m_inbox: table
     * m_neighbours: table
     *          > m_agentID
     *          > m_mass
     *          > m_strength
     *          > m_position
     *          > m_state
     *          > m_attributes
     * m_faces: table?
     */

    // std::cout << "Agent " << this << ": Executing" << std::endl;


    // Checking if the stack is not empty, it might contain any error
    if (lua_gettop(s_luaState) > 0)
    {
        std::cout << "Agent: LUA ERROR: " << lua_tostring(s_luaState,-1) << std::endl;
        return;
    }

    // Stack[1]: global function name
    lua_getglobal(s_luaState,m_brain.c_str());

    // Stack[2]: m_agentID
    lua_pushnumber(s_luaState,m_agentID);

    // Stack[3]: m_position
    ngl::Vec4 position = m_transformation.getPosition();
    lua_createtable(s_luaState,3,0);

    lua_pushstring(s_luaState,"x");
    lua_pushnumber(s_luaState,position.m_x);
    lua_settable(s_luaState,3);
    lua_pushstring(s_luaState,"y");
    lua_pushnumber(s_luaState,position.m_y);
    lua_settable(s_luaState,3);
    lua_pushstring(s_luaState,"z");
    lua_pushnumber(s_luaState,position.m_z);
    lua_settable(s_luaState,3);

    // Stack[4]: m_strength
    lua_pushnumber(s_luaState,m_strength);

    // Stack[5]: m_maxStrength
    lua_pushnumber(s_luaState,m_maxStrength);

    // Stack[6]: m_velocity
    lua_createtable(s_luaState,3,0);

    lua_pushstring(s_luaState,"x");
    lua_pushnumber(s_luaState,m_velocity.m_x);
    lua_settable(s_luaState,6);
    lua_pushstring(s_luaState,"y");
    lua_pushnumber(s_luaState,m_velocity.m_y);
    lua_settable(s_luaState,6);
    lua_pushstring(s_luaState,"z");
    lua_pushnumber(s_luaState,m_velocity.m_z);
    lua_settable(s_luaState,6);

    // Stack[7]: m_state
    lua_pushstring(s_luaState,m_state.c_str());

    // Stack[8]: m_attributes
    int nAttributes = m_attributes.size();
    lua_createtable(s_luaState,nAttributes,0);

    std::pair<std::string,std::string> attribute;
    std::map<std::string,std::string>::const_iterator endAttribute = m_attributes.end();
    for(std::map<std::string,std::string>::const_iterator currentAttribute = m_attributes.begin();
        currentAttribute!=endAttribute; ++currentAttribute)
    {
        attribute = (*currentAttribute);
        lua_pushstring(s_luaState, attribute.first.c_str());
        lua_pushstring(s_luaState, attribute.second.c_str());
        lua_settable(s_luaState, 8);
    }

    // Stack[9]: inbox
    // be careful with the stack (20 slots)
    message msge;
    int index = 1;
    int nMessage = m_inbox.size();
    lua_createtable(s_luaState,nMessage,0);

    std::vector<message>::const_iterator endMessage = m_inbox.end();
    for(std::vector<message>::const_iterator currentMessage = m_inbox.begin();
        currentMessage!=endMessage; ++currentMessage)
    {
        /**
         * Each message is gonna be a table stored temporaly at Stack[10],
         * but then it will be added to the table of message at Stack[9]
         */
        msge = (*currentMessage);
        lua_createtable(s_luaState,4,0);
        lua_pushstring(s_luaState, "agentID");
        lua_pushinteger(s_luaState, msge.agentID);
        lua_settable(s_luaState, 10);
        lua_pushstring(s_luaState, "label");
        lua_pushstring(s_luaState, msge.label.c_str());
        lua_settable(s_luaState, 10);

        //position key at Stack[11]
        position = msge.position;
        lua_pushstring(s_luaState, "position");
        //table for position at Stack[12]
        lua_createtable(s_luaState,3,0);
        lua_pushstring(s_luaState,"x");
        lua_pushnumber(s_luaState,position.m_x);
        lua_settable(s_luaState,12);
        lua_pushstring(s_luaState,"y");
        lua_pushnumber(s_luaState,position.m_y);
        lua_settable(s_luaState,12);
        lua_pushstring(s_luaState,"z");
        lua_pushnumber(s_luaState,position.m_z);
        lua_settable(s_luaState,12);
        //setting on the table message
        lua_settable(s_luaState,10);

        lua_pushstring(s_luaState, "strength");
        lua_pushnumber(s_luaState, msge.strength);
        lua_settable(s_luaState, 10);

        lua_rawseti(s_luaState,9,index);
        ++index;
    }

    // Stack[10]: neighbours
    Agent* agent;
    ngl::Vec4 velocity;
    index = 1;
    int nNeighbours = m_neighbours.size();
    std::map<std::string,std::string> neighbourAttributes;
    lua_createtable(s_luaState,nNeighbours,0);

    std::vector<Agent*>::const_iterator endAgent = m_neighbours.end();
    for(std::vector<Agent*>::const_iterator currentAgent = m_neighbours.begin(); currentAgent!=endAgent; ++currentAgent)
    {
        /**
         * Each agent is gonna be a table stored temporaly at Stack[11],
         * but then it will be added to the table of neightbours at Stack[10]
         */

        agent = (*currentAgent);
        lua_createtable(s_luaState,2,0);

        // agentID
        lua_pushstring(s_luaState,"agentID");
        lua_pushnumber(s_luaState,agent->getAgentID());
        lua_settable(s_luaState,11);

        // mass
        lua_pushstring(s_luaState,"mass");
        lua_pushnumber(s_luaState,agent->getMass());
        lua_settable(s_luaState,11);

        // strength
        lua_pushstring(s_luaState,"strength");
        lua_pushnumber(s_luaState,agent->getStrength());
        lua_settable(s_luaState,11);

        // position
        position = agent->getPosition();
        // m_position key at Stack[12]
        lua_pushstring(s_luaState,"position");
        // Position table at Stack[13]
        lua_createtable(s_luaState,3,0);
        lua_pushstring(s_luaState,"x");
        lua_pushnumber(s_luaState,position.m_x);
        lua_settable(s_luaState,13);
        lua_pushstring(s_luaState,"y");
        lua_pushnumber(s_luaState,position.m_y);
        lua_settable(s_luaState,13);
        lua_pushstring(s_luaState,"z");
        lua_pushnumber(s_luaState,position.m_z);
        lua_settable(s_luaState,13);
        // Add position table to the Agent-table
        lua_settable(s_luaState,11);

        // velocity
        velocity = agent->getVelocity();
        // m_position key at Stack[12]
        lua_pushstring(s_luaState,"velocity");
        // Position table at Stack[13]
        lua_createtable(s_luaState,3,0);
        lua_pushstring(s_luaState,"x");
        lua_pushnumber(s_luaState,velocity.m_x);
        lua_settable(s_luaState,13);
        lua_pushstring(s_luaState,"y");
        lua_pushnumber(s_luaState,velocity.m_y);
        lua_settable(s_luaState,13);
        lua_pushstring(s_luaState,"z");
        lua_pushnumber(s_luaState,velocity.m_z);
        lua_settable(s_luaState,13);
        // Add position table to the Agent-table
        lua_settable(s_luaState,11);


        // state
        lua_pushstring(s_luaState,"state");
        lua_pushstring(s_luaState,agent->getState().c_str());
        lua_settable(s_luaState,11);


        // Attributes
        neighbourAttributes = agent->getAttributes();
        nAttributes = neighbourAttributes.size();
        // m_messages key at Stack[12]
        lua_pushstring(s_luaState,"attributes");
        // Messages table at Stack[13]
        lua_createtable(s_luaState,nAttributes,0);

        endAttribute = neighbourAttributes.end();
        for(std::map<std::string,std::string>::const_iterator currentAttribute = neighbourAttributes.begin();
            currentAttribute!=endAttribute; ++currentAttribute)
        {
            attribute = (*currentAttribute);
            lua_pushstring(s_luaState, attribute.first.c_str());
            lua_pushstring(s_luaState, attribute.second.c_str());
            lua_settable(s_luaState, 13);
        }
        // Add message table to the Agent-table
        lua_settable(s_luaState,11);

        //Add neighbour table
        lua_rawseti(s_luaState,10,index);
        ++index;
    }


    // Take a lot of care here (func+10)
    int errfunc = 0;
    int errorCode = lua_pcall(s_luaState,9,5,errfunc);

    if (errorCode)
    {
        std::cout << "Agent: LUA RUNTIME ERROR! :" << lua_tostring(s_luaState,-1) << std::endl;
        lua_remove(s_luaState,-1);
        return;
    }


    /*
    std::cout << lua_typename(s_luaState,lua_type(s_luaState,1))
              << lua_typename(s_luaState,lua_type(s_luaState,2))
              << lua_typename(s_luaState,lua_type(s_luaState,3))
              << lua_typename(s_luaState,lua_type(s_luaState,4))  << std::endl;
              */

    // Returned values
    /**
     * force: table
     * heading: table
     * strength: number
     * state: string
     * messages: table
     */

    // Force at Stack[1]
    ngl::Vec4 agentForce(0,0,0,0);
    lua_pushnil(s_luaState);
    if (lua_next(s_luaState,1))
    {
        agentForce.m_x = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,1);
        lua_next(s_luaState,1);
        agentForce.m_y = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,1);
        lua_next(s_luaState,1);
        agentForce.m_z = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,2);
    }

    // ADDING MOVEMENT FORCE
    m_totalForce += agentForce;

    // Heading at Stack[2]
    m_heading = 0;
    lua_pushnil(s_luaState);
    if (lua_next(s_luaState,2))
    {
        m_heading.m_x = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,1);
        lua_next(s_luaState,2);
        m_heading.m_y = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,1);
        lua_next(s_luaState,2);
        m_heading.m_z = lua_tonumber(s_luaState,-1);
        lua_pop(s_luaState,2);
    }

    // Strength at Stack[3]
    m_strength = lua_tonumber(s_luaState,3);

    // CLAMPING STRENGTH
    m_strength = fminf(m_strength,m_maxStrength);

    // State at Stack[4]
    m_state = std::string(lua_tostring(s_luaState,4));

    // Messages to send at Stack[5]
    message msg;
    int neighbourIndex;
    msg.agentID = m_agentID;
    msg.position = m_transformation.getPosition();
    msg.strength = m_strength;
    lua_pushnil(s_luaState);
    while (lua_next(s_luaState,5))
    {
        /* One bug found here:
         * next does not iterate over nil entries on a table,
         * thus you cannot count the iterations
         * And one extra one!
         * In C++ index start in 0, but in lua they do in 1..
         * cIndex = luaIndex -1
         */
        neighbourIndex = lua_tointeger(s_luaState,-2)-1;
        lua_pushnil(s_luaState);
        while (lua_next(s_luaState,7))
        {
            msg.label = std::string(lua_tostring(s_luaState,9));
            lua_pop(s_luaState,1);
            m_neighbours[neighbourIndex]->sendMessage(msg);
        }
        lua_pop(s_luaState,1);
        ++neighbourIndex;
    }

    // Leave the lua stack as you found it! (empty)
    lua_pop(s_luaState,5);

    // SUBSTRACTING FRICTION FORCE
    //m_totalForce -= s_friction*m_velocity;

    // CALCULATE NEW VELOCITY WITH THE FORCE
    ngl::Vec4 acceleration = m_totalForce/m_mass;
    m_velocity += acceleration;

    // Clamping m_velocity to m_maxSpeed
    float speed = m_velocity.length();
    if (speed>m_maxSpeed)
    {
        m_velocity = m_velocity/speed * m_maxSpeed;
    }

    /**
     * CALCULATE NEW ROTATION
     * It is calculated just in Y, for other specific rotations
     * it should handled by the behaviour with the heading
     */
    ngl::Vec4 rotation = 0;

    ngl::Vec3 heading;
    if (m_heading.length()>0)
        heading = m_heading;
    else
        heading = m_velocity;

    if (heading.m_x != 0)
        rotation.m_y = atan2(-heading.m_z,heading.m_x) * 180/M_PI;
    else
    {
        if (heading.m_z<0)
            rotation.m_y = 90;
        else
            rotation.m_y = -90;
    }

    m_previousTransform = m_transformation;
    m_transformation.addPosition(m_velocity*s_step);
    m_transformation.setRotation(rotation);


    // DO NOT FORGET RESET MEMBERS WHICH NEED TO BE
    m_totalForce = 0;
    m_inbox.clear();

}
