#ifndef AGENT_H
#define AGENT_H

#include <set>
#include <sstream>
#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}


class Agent
{

private:
    static lua_State* s_luaState;

    bool m_active;
    float m_mass;
    float m_strength;
    float m_maxStrength;
    float m_visionRadius;
    ngl::Vec3 m_direction;
    ngl::Vec3 m_totalForce;
    ngl::Vec3 m_velocity;
    float m_maxSpeed;
    ngl::Transformation m_transformation;
    ngl::Transformation m_previousTransform;
    std::string m_state;
    float m_visionRange;
    std::string m_brain;
    std::map<std::string,std::string> m_labels;
    std::string m_primitive;

    std::vector<Agent*> m_neighbours;
    std::vector<Agent*> m_watchedAgents;

    void flockingForce();
    void draggingForce();

public:
    Agent();
    Agent(ngl::Vec3 _pos, std::string _flock, std::string _brain);
    ~Agent() { }

    void static setLuaState(lua_State* _luaState) { s_luaState = _luaState; }

    void setActive(bool _active) { m_active = _active; }
    void setMass(float _mass) { m_mass = _mass; }
    void setMaxStrength(float _maxStrength) { m_maxStrength = _maxStrength; m_strength = m_maxStrength; }
    void setMaxSpeed(float _maxSpeed) { m_maxSpeed = _maxSpeed; }
    void setVisionRadius(float _visionRadius) { m_visionRadius = _visionRadius; }
    void setPosition(ngl::Vec3 _pos) { m_transformation.setPosition(_pos); }
    void setNeighbours(const std::vector<Agent*> &_neighbours) { m_neighbours = _neighbours; }
    void setBrain(std::string _brain) { m_brain = _brain; }
    void addLabel(std::string _key, std::string _value) { m_labels[_key] = _value; }

    ngl::Vec4 getPosition() const { return m_transformation.getPosition(); }
    ngl::Vec4 getPreviousPos() const { return m_previousTransform.getPosition(); }
    ngl::Transformation getTransform() const { return m_transformation; }
    int getVisionRadius() const { return m_visionRadius; }
    std::string getBrain() const { return m_brain; }

    void execute();
    float distance(const Agent *_agent) const;
    void print() const;

};

#endif // AGENT_H
