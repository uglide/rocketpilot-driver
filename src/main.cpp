
#include <QApplication>
#include <QMainWindow>
#include <QMainWindow>

#include "qttestability.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QMainWindow win;
    win.show();
    qt_testability_init();
    return app.exec();
}
