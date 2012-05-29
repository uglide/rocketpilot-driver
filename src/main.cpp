/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

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
