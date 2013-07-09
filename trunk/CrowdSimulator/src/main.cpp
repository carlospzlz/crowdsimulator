#include "ngl/Vec2.h"
#include "CrowdEngine.h"

int main(int argc, char **args)
{

    if (argc=0)
    {
        std::cout << "GRAPHIC MODE" << std::endl;

        QApplication app(argc, args);

        MainWindow win;
        win.show();
        return app.exec();;
    }

    std::cout << "TERMINAL/DEBUGGING MODE" << std::endl;

    CrowdEngine myCrowdEngine;

    myCrowdEngine.createFlock(atoi(args[1]),atoi(args[2]),ngl::Vec2(0,0));

    //myCrowdEngine.printAgents();

    //myCrowdEngine.printCellPartition();

    myCrowdEngine.update();  
    myCrowdEngine.update();
    myCrowdEngine.update();

    QGLWidget w;
    w.paint

    std::cout << QT_VERSION << QT_VERSION_STR << std::endl;


    return 0;
}
