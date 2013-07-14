#include <QApplication>
#include "MainWindow.h"

#include "ngl/Vec2.h"
#include "CrowdEngine.h"

int main(int argc, char **args)
{

    if ( argc == 1 )
    {
        std::cout << "GRAPHIC MODE" << std::endl;

        QApplication app(argc, args);

        MainWindow win;
        win.show();
        return app.exec();;
    }
    else if ( argc >= 2)
    {

        std::cout << "TERMINAL/DEBUGGING MODE" << std::endl;

        CrowdEngine myCrowdEngine;
        Agent* myAgent = new Agent();

        myCrowdEngine.loadBrain("boid");
        myCrowdEngine.loadBrain("leaderBoid");
        myCrowdEngine.loadBrain("empty");

        myAgent->setBrain("leaderBoid");
        myAgent->addAttribute("flock","testFlock");
        myAgent->setPosition(ngl::Vec3(0,0,0));
        myCrowdEngine.addAgent(myAgent);

        myCrowdEngine.createRandomFlock(atoi(args[1]),atoi(args[2]),ngl::Vec2(0,0),"testFlock");

        myCrowdEngine.printAgents();

        //myCrowdEngine.printCellPartition();

        myCrowdEngine.update();
        myCrowdEngine.printAgents();
    }


    return 0;
}
