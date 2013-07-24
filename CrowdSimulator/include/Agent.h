#ifndef AGENT_H
#define AGENT_H

#include <set>
#include <sstream>
#include <math.h>
#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

struct message
{
    int agentID;
    std::string label;
    ngl::Vec3 position;
    float strength;
};

class Agent
{

private:
    static lua_State* s_luaState;
    static int s_numberOfAgents;
    static float s_step;
    static float s_friction;

    int m_agentID;
    float m_mass;
    float m_strength;
    float m_maxStrength;
    float m_visionRadius;
    ngl::Vec4 m_totalForce;
    ngl::Vec4 m_velocity;
    ngl::Vec4 m_heading;
    float m_maxSpeed;
    ngl::Transformation m_transformation;
    ngl::Transformation m_previousTransform;
    std::string m_state;
    std::string m_brain;
    std::vector<message> m_inbox;
    std::map<std::string,std::string> m_attributes;
    std::vector<Agent*> m_neighbours;
    float m_collisionRadius;

public:
    Agent();
    Agent(ngl::Vec4 _pos, std::string _flock, std::string _brain);
    Agent(const Agent &_agent);
    ~Agent() { }

    void static setLuaState(lua_State* _luaState) { s_luaState = _luaState; }
    void static setStep(float _step) { s_step = _step; }
    void static setFriction(float _friction) { s_friction = _friction; }

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

    void execute();
    void sendMessage(message _message) { m_inbox.push_back(_message); }
    float distance(const Agent *_agent) const;
    void scaleCollisionRadius(float _scale) { m_collisionRadius = m_mass*_scale; }
    void print() const;

};

#endif // AGENT_H
