#include "ngl/Vec2.h"
#include "CrowdEngine.h"

int main()
{
    /*
    std::pair<int,int> a(0,0);
    std::pair<int,int> b(0,0);

    std::cout << "Testing equality of vec2" << std::endl;

    if (a==b)
        std::cout << "they are equal" << std::endl;

    std::map<std::pair<int,int>,int> map;

    for (int i=0; i<1000; ++i)
        for (int j=0; j<1000; ++j)
            map[std::pair<int,int>(i,j)] = i*j;

    for (int i=0; i<1000; ++i)
        for (int j=0; j<1000; ++j)
            map[std::pair<int,int>(i,j)];
            //std::cout << map[std::pair<int,int>(i,j)] << std::endl;

    std::cout << "accessing...\n" << map.at(b) << std::endl;
    */

    CrowdEngine myCrowdEngine;

    myCrowdEngine.createFlock(100,100,ngl::Vec2(0,0));

    //myCrowdEngine.printAgents();

    //myCrowdEngine.printCellPartition();

    myCrowdEngine.update();  
    myCrowdEngine.update();
    myCrowdEngine.update();


    return 0;
}
