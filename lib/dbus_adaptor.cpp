/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "dbus_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QDebug>

/*
 * Implementation of adaptor class AutopilotAdaptor
 */

AutopilotAdaptor::AutopilotAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

AutopilotAdaptor::~AutopilotAdaptor()
{
    // destructor
}

void AutopilotAdaptor::GetState(const QString &piece, const QDBusMessage &message)
{
    message.setDelayedReply(true);
    QDBusMessage reply = message.createReply();

    // handle method call com.canonical.Unity.Debug.Introspection.GetState
    QMetaObject::invokeMethod(
                parent(),
                "GetState",
                Qt::QueuedConnection,
                Q_ARG(QString, piece),
                Q_ARG(QDBusMessage, reply)
                );
}

