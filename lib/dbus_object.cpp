/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "dbus_object.h"
#include "introspection.h"

#include <QList>
#include <QVariantMap>
#include <QDebug>
#include <QApplication>
#include <QDBusConnection>
#include <QThread>

DBusObject::DBusObject(QObject *parent)
    : QObject(parent)
{
}

void DBusObject::GetState(const QString &piece, const QDBusMessage &msg)
{
    _queries.append(Query(piece, msg));

    // We need to surrender to the Qt event loop, so we do the processing
    // via a queued slot connection:
    QMetaObject::invokeMethod(
                this,
                "ProcessQuery",
                Qt::QueuedConnection
                );

    qDebug("End of GetState");
}

void DBusObject::ProcessQuery()
{
    qDebug("Start of ProcessQuery");
    Query query = _queries.takeFirst();
    QList<QVariant> state = Introspect(query.first);

    QDBusMessage msg = query.second;
    msg << QVariant(state);

    QDBusConnection::sessionBus().send(msg);
    qDebug("Reply sent.");
}

