#include "window.h"
#include <QApplication>
#include <iostream>
#include "utils.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;

    POLL_DEBUG(std::cout, "Poll data path: " << POLL_DATA_PATH);
    POLL_DEBUG(std::cout, "Poll shader path: " << POLL_SHADER_PATH);

    w.show();


    return a.exec();

}
