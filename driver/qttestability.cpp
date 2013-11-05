/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "qttestability.h"
#include "dbus_adaptor.h"
#include "dbus_adaptor_qt.h"
#include "dbus_object.h"
#include "qtnode.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtDBus>

const QString DBUS_OBJECT_PATH("/com/canonical/Autopilot/Introspection");

void qt_testability_init(void)
{
    qDebug() << "Testability driver loaded. Wire protocol version is" << AutopilotAdaptor::WIRE_PROTO_VERSION << ".";
    qDBusRegisterMetaType<NodeIntrospectionData>();
    qDBusRegisterMetaType<QList<NodeIntrospectionData> >();

    DBusObject* obj = new DBusObject;
    new AutopilotAdaptor(obj);
    new AutopilotQtSpecificAdaptor(obj);

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.registerObject(DBUS_OBJECT_PATH, obj))
    {
        qDebug("Unable to register object on D-Bus! Testability interface will not be available.");
    }
}
