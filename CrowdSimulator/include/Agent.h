#ifndef AGENT_H
#define AGENT_H

#include "ngl/Vec3.h"
#include "ngl/Transformation.h"

class Agent
{

private:
    bool m_active;
    float m_mass;
    float m_strength;
    float m_maxStrength;
    ngl::Vec3 m_totalForce;
    ngl::Vec3 m_velocity;
    float m_maxSpeed;
    ngl::Transformation m_transformation;
    ngl::Transformation m_previousTransform;
    int m_flockID;
    std::string m_state;
    float m_visionRange;
    std::string m_label;
    std::string m_primitive;

    std::vector<Agent*> m_neighbours;

    void flockingForce();
    void draggingForce();

public:
    Agent();
    Agent(ngl::Vec3 _pos, int _fid);
    ~Agent() { }

    //void setPosition(ngl::Vec3 _pos); { m_transformation.setPosition(_pos); }
    //void setFlock(int _fid) { m_flockID = _fid; }

    ngl::Vec4 getPosition() { return m_transformation.getPosition(); }
    ngl::Vec4 getPreviousPos() { return m_previousTransform.getPosition(); }

    void execute();
    void print();

};

#endif // AGENT_H
