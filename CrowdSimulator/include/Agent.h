#ifndef AGENT_H
#define AGENT_H

/**
 * @file Agent.h
 * @brief This implements the body of the agent, which includes all the
 * physical properties, neighbourhood, messages, transformation, etc.
 * and knows about the brain, a lua function which is called
 * every time an update occurs
 */

#include <set>
#include <sstream>
#include <math.h>
#include "ngl/Vec3.h"
#include "ngl/Transformation.h"
#include "ngl/Obj.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

/**
 * @struct message
 * @brief Specifies the packet of data that conforms a message,
 * and which allows the communication among agents
 */
struct message
{
    int agentID;
    std::string label;
    ngl::Vec3 position;
    float strength;
};

/**
 * @class Agent
 * @brief This class implements the body of the agent, which includes all the
 * physical properties, neighbourhood, messages, transformation, etc.
 * and knows about the brain, a lua function which is called
 * every time an update occurs
 */
class Agent
{

private:
    /**
     * @brief Lua state which holds the scripted brain functions
     */
    static lua_State* s_luaState;
    /**
     * @brief Number of the current agents, used to associate an
     * agent identifier
     */
    static int s_numberOfAgents;
    /**
     * @brief This controls the rate of change of the position of the agent.
     * This attribute is very important and influences the simulation
     * very much
     */
    static float s_step;

    /**
     * @brief The unique agent ID which differentiates it from any other
     */
    int m_agentID;
    /**
     * @brief The mass of the agent
     */
    float m_mass;
    /**
     * @brief The current strength of the agent. This attribute is mutable
     */
    float m_strength;
    /**
     * @brief The maximum strength the agent has in full recovery state
     */
    float m_maxStrength;
    /**
     * @brief This determines the portion of the world the agent is aware of
     */
    float m_visionRadius;
    /**
     * @brief Stores the total force applied on this agent
     */
    ngl::Vec4 m_totalForce;
    /**
     * @brief Current velocity of the agent
     */
    ngl::Vec4 m_velocity;
    /**
     * @brief Current heading of the agent. It often points the same direction
     * than the velocity, but this does not have to happen always
     */
    ngl::Vec4 m_heading;
    /**
     * @brief Maximum speed this agent can move with
     */
    float m_maxSpeed;
    /**
     * @brief Current transformation of the agent. This includes translation
     * rotation and scale
     */
    ngl::Transformation m_transformation;
    /**
     * @brief Transformation of the agent in the previous tick
     */
    ngl::Transformation m_previousTransform;
    /**
     * @brief State of the agent. Information needed for the brain in order
     * to take decisions
     */
    std::string m_state;
    /**
     * @brief Name of the brain, which correspondes with the lua function
     * that encapsulates the "intelligent" routine
     */
    std::string m_brain;
    /**
     * @brief Vector with all the incoming messages from other agents
     */
    std::vector<message> m_inbox;
    /**
     * @brief Map which stores extra attributes to provide more information
     * to the brain
     */
    std::map<std::string,std::string> m_attributes;
    /**
     * @brief Vector with the current neighbours of the agent
     */
    std::vector<Agent*> m_neighbours;
    /**
     * @brief Radius of the agent in order to handle the collisions
     */
    float m_collisionRadius;
    /**
     * @brief Dummy used when drawing the agent
     */
    ngl::Obj* m_dummy;

public:
    Agent();
    Agent(ngl::Vec4 _pos, std::string _flock, std::string _brain);
    Agent(const Agent &_agent);
    ~Agent() { }

    void static setLuaState(lua_State* _luaState) { s_luaState = _luaState; }
    void static setStep(float _step) { s_step = _step; }

    float static getStep() { return s_step; }

    void setMass(float _mass);
    void setMaxStrength(float _maxStrength) { m_maxStrength = _maxStrength; m_strength = m_maxStrength; }
    void setMaxSpeed(float _maxSpeed) { m_maxSpeed = _maxSpeed; }
    void setHeading(ngl::Vec4 _heading);
    void setVisionRadius(float _visionRadius) { m_visionRadius = _visionRadius; }
    void setPosition(ngl::Vec4 _pos) { m_transformation.setPosition(_pos); }
    void setVelocity(ngl::Vec4 _vel) { m_velocity = _vel; }
    void setNeighbours(const std::vector<Agent*> &_neighbours) { m_neighbours = _neighbours; }
    void setBrain(std::string _brain) { m_brain = _brain; }
    void setState(std::string _state) { m_state = _state; }
    void setDummy(ngl::Obj *_dummy) { m_dummy = _dummy; }
    void addAttribute(std::string _key, std::string _value) { m_attributes[_key] = _value; }

    float getAgentID() const { return m_agentID; }
    ngl::Vec4 getPosition() const { return m_transformation.getPosition(); }
    ngl::Vec4 getPreviousPos() const { return m_previousTransform.getPosition(); }
    float getMass() const { return m_mass; }
    float getStrength() const { return m_strength; }
    float getMaxStrength() const { return m_maxStrength; }
    ngl::Vec4 getVelocity() const { return m_velocity; }
    float getMaxSpeed() const { return m_maxSpeed; }
    std::string getState() const { return m_state; }
    ngl::Transformation getTransform() const { return m_transformation; }
    int getVisionRadius() const { return m_visionRadius; }
    std::string getBrain() const { return m_brain; }
    std::map<std::string,std::string> getAttributes() const { return m_attributes; }
    float getCollisionRadius() const { return m_collisionRadius; }
    std::vector<Agent*> getNeighbours() const { return m_neighbours; }
    ngl::Vec4 getHeading() const { return m_heading; }
    ngl::Obj* getDummy() const { return m_dummy; }

    /**
     * @brief Executes and therefore updates the state of the agent.
     * This is where the routine of the brain is called
     */
    void execute();
    /**
     * @brief Requests to apply a force on this agent
     */
    void applyForce(ngl::Vec4 _force) {m_totalForce += _force; }
    /**
     * @brief Sends a message to this agent
     */
    void sendMessage(message _message) { m_inbox.push_back(_message); }
    /**
     * @brief Calculates the distance with _agent
     */
    float distance(const Agent *_agent) const;
    /**
     * @brief Multiply the collision radius by _scale
     */
    void scaleCollisionRadius(float _scale) { m_collisionRadius = m_mass*_scale; }
    /**
     * @brief print in the standard output information about the agent
     */
    void print() const;

};

#endif // AGENT_H
