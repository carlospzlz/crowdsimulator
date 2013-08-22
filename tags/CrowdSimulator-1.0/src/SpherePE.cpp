#include <SpherePE.h>

void SpherePE::checkCollision(Agent *_a1, Agent *_a2)
{
    /**
     * IMPORTANT NOTE
     * the position of one agent is stuck to the ground,
     * so in order to use a bounding sphere collision checking
     * we need to use the center of mass. Having into account
     * that in our dummies that the mass is equivalent to the radius
     * that covers that mass this modification needs to be done:
     * position.m_y = position.m_y + mass
     */

    ngl::Vec3 centreOfMass1, centreOfMass2;
    ngl::Vec3 distanceVector;
    float distance, radiusSum;

    centreOfMass1 = _a1->getPosition();
    centreOfMass1.m_y += _a1->getMass();

    centreOfMass2 = _a2->getPosition();
    centreOfMass2.m_y += _a2->getMass();

    distanceVector.m_x = centreOfMass2.m_x-centreOfMass1.m_x;
    distanceVector.m_y = centreOfMass2.m_y-centreOfMass1.m_y;
    distanceVector.m_z = centreOfMass2.m_z-centreOfMass1.m_z;
    distance = sqrt(distanceVector.m_x*distanceVector.m_x +
                    distanceVector.m_y*distanceVector.m_y +
                    distanceVector.m_z*distanceVector.m_z);

    radiusSum = _a1->getCollisionRadius() + _a2->getCollisionRadius();

    //if distance is 0 failed assert is recieved from Vec4::normalize (len!=0) ¿?
    if (distance < s_epsilon)
    {
        std::cout << "SpherePE: WARNING: Agent " << _a1->getAgentID()
                  << " and " << _a2->getAgentID() << " are overlapped" << std::endl;
        return;
    }

    if (distance < radiusSum)
    {
        //THERE IS COLLISION
        /**
         * The velocity vector is projected on to the normal which has
         * as normal the distance vector.
         * projection = velocity - (velocity.normal)*normal
         * http://my.safaribooksonline.com/book/illustration-and-graphics/
         * 9781558605947/chapter-12-miscellaneous-3d-problems/head1_id_93
         */

        float magnitude;
        float step = Agent::getStep();
        float angleA1, angleA2;
        ngl::Vec3 normal;

        //FIXING AGENT 1
        ngl::Vec4 velocityA1;
        velocityA1 = _a1->getVelocity();

        if (velocityA1.length() > 0)
        {
            angleA1 = velocityA1.angleBetween(distanceVector);

            if (angleA1 < M_PI/2.0)
            {
                // FIX!
                // fixing velocity of Agent1
                normal = -distanceVector/distanceVector.length();
                magnitude = velocityA1.dot(normal);
                velocityA1 = velocityA1 - magnitude*normal;
                _a1->setVelocity(velocityA1);
                // fixing position of Agent1
                _a1->setPosition(_a1->getPreviousPos()+velocityA1*step);
            }
        }

        //FIXING AGENT 2
        ngl::Vec4 velocityA2;
        velocityA2 = _a2->getVelocity();

        if (velocityA2.length() > 0)
        {
            angleA2 = velocityA2.angleBetween(-distanceVector);

            if (angleA2 < M_PI/2.0)
            {
                //FIX AGENT 2
                //fixing velocity of Agent2
                normal = distanceVector/distanceVector.length();
                magnitude = velocityA2.dot(normal);
                velocityA2 = velocityA2 - magnitude*normal;
                _a2->setVelocity(velocityA2);
                //fixing position of Agent2
                _a2->setPosition(_a2->getPreviousPos()+velocityA2*step);
            }
        }
    }

}
