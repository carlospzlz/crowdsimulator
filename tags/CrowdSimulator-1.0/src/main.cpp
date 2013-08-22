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
    else
    {
        std::cout << "TEXT MODE" << std::endl;
	/*
	 * For testing and debugging purpose
	 */
    }


    return 0;
}
