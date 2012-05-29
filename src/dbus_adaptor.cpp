/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "src/dbus_adaptor.h"
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
    // handle method call com.canonical.Unity.Debug.Introspection.GetState
    QList<QVariant> state;
    QMetaObject::invokeMethod(parent(),
        "GetState",
        Q_RETURN_ARG(QList<QVariant>, state),
        Q_ARG(QString, piece));


    qDebug() << "Inside GetState()";
    qDebug() << "Message is:" << message;

    QVariant reply(state);
    QDBusConnection::sessionBus().send(message.createReply(reply));
}

