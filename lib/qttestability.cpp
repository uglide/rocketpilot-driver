/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "qttestability.h"
#include "dbus_adaptor.h"
#include "dbus_object.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtDBus>

void qt_testability_init(void)
{
    QCoreApplication* app = QCoreApplication::instance();
    qDebug() << "App instance is: " << app;

    DBusObject* obj = new DBusObject;
    new AutopilotAdaptor(obj);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService("com.canonical.Autopilot"))
    {
        qDebug("Unable to register service!");
    }
    if (!connection.registerObject("/", obj))
    {
        qDebug("Unable to register object!");
    }
}
