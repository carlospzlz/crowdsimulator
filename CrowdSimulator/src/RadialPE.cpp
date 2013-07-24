#include "RadialPE.h"

void RadialPE::checkCollision(Agent *a1, Agent *a2)
{
    float distance = a1->distance(a2);
    float radiusSum = a1->getCollisionRadius()+a2->getCollisionRadius();

    if (distance < radiusSum)
    {
        //THERE IS COLLISION
        //std::cout << "COLLISION!" << std::endl;
        a1->setPosition(a1->getPreviousPos());
        a2->setPosition(a2->getPreviousPos());


        ngl::Vec4 distanceVector;
        distanceVector.m_x = a2->getPosition().m_x - a1->getPosition().m_x;
        distanceVector.m_z = a2->getPosition().m_z - a1->getPosition().m_z;
        //vector tangent to the collision
        ngl::Vec4 tangentVector;
        tangentVector.m_x = - distanceVector.m_z;
        tangentVector.m_z = distanceVector.m_x;
        //normalize
        tangentVector = tangentVector / tangentVector.length();

        //FIXING AGENT1
        ngl::Vec4 velocityA1;
        velocityA1.m_x = a1->getVelocity().m_x;
        velocityA1.m_z = a1->getVelocity().m_z;

        float magnitude = velocityA1.dot(tangentVector);
        velocityA1 = magnitude*tangentVector;
        a1->setVelocity(velocityA1);
        // fixing position of Agent1
        float step = Agent::getStep();
        a1->setPosition(a1->getPreviousPos()+velocityA1*step);

        //FIXING AGENT2
        ngl::Vec4 velocityA2;
        velocityA1.m_x = a2->getVelocity().m_x;
        velocityA1.m_z = a2->getVelocity().m_z;

        magnitude = velocityA2.dot(tangentVector);
        velocityA2 = magnitude*tangentVector;
        a2->setVelocity(velocityA2);
        // fixing position of Agent2
        a2->setPosition(a2->getPreviousPos()+velocityA2*step);


    }
}
