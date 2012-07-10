/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include <QTest>

#include "test_dbus_introspection.h"

int main(int argc, char **argv)
{
    TestDBusIntrospection tests;
    return QTest::qExec(&tests, argc, argv);
}
