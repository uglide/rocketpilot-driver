/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "dbus_object.h"
#include "introspection.h"
#include "qtnode.h"

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

void DBusObject::RegisterSignalInterest(int object_id, QString signal_name)
{
    SignalId signal(object_id, signal_name);
    if (signal_watchers_.contains(signal))
    {
        qDebug() << "Already watching signal" << signal_name << "on object with id" << object_id;
        return;
    }

    QString query = QString("//*[id=%1]").arg(object_id);
    QList<QtNode::Ptr> objects = GetNodesThatMatchQuery(query);

    if (objects.isEmpty())
    {
        qWarning() << "No Object with with id" << object_id << "While trying to monitor signal" << signal_name;
        return;
    }

    if (objects.count() != 1)
    {
        qWarning() << "More than one object has id" << object_id << ". This should never happen, and indicates a bug in autopilot-qt.";
    }

    QtNode::Ptr node = objects.takeFirst();
    QObject* obj = node->getWrappedObject();

    QString munged_signal_name = QString("2%1").arg(signal_name);
    SignalSpyPtr signal_spy(new QSignalSpy(obj, munged_signal_name.toAscii().data()));
    if (signal_spy->isValid())
    {
        signal_watchers_[signal] = signal_spy;
        qDebug() << "Now watching for emissions of the" << signal_name << "signal on object with id" << object_id;
    }
    else
    {
        qWarning() << "Signal name was not vlaid.";
    }
}

void DBusObject::GetSignalEmissions(int object_id, QString signal_name, const QDBusMessage &message)
{
    QDBusMessage reply = message.createReply();
    SignalId signal(object_id, signal_name);

    if (signal_watchers_.contains(signal))
    {
        SignalSpyPtr signal_spy = signal_watchers_[signal];
        QList<QVariant> signal_emit_list;
        qDebug() << "Signal emissionss" << *signal_spy;
        for (int i = 0; i < signal_spy->length(); ++i)
        {
            QList<QVariant> signal_emission = signal_spy->at(i);
            qDebug() << "Packing" << signal_emission << "Into QVariant";
            signal_emit_list.append(QVariant(signal_emission));
        }

        qDebug() << "Packing" << signal_emit_list << "into DBus reply.";
        reply << QVariant(signal_emit_list);
    }
    else
    {
        qDebug() << "That signal was never registered for watching.";
    }
    if (QDBusConnection::sessionBus().send(reply))
        qDebug("Reply sent.");
    else
        qDebug("Error on reply send.");
}

void DBusObject::ListSignals(int object_id, const QDBusMessage& message)
{
    QString query = QString("//*[id=%1]").arg(object_id);
    QList<QtNode::Ptr> objects = GetNodesThatMatchQuery(query);

    if (objects.isEmpty())
    {
        qWarning() << "No Object with with id" << object_id << "While trying to list signals";
        return;
    }

    QObject *object = objects.takeFirst()->getWrappedObject();
    const QMetaObject *meta = object->metaObject();
    QList<QVariant> signal_list;
    do
    {
        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
        {
            QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Signal)
            {
                QString signature = QString::fromLatin1(method.signature());
                signal_list.append(QVariant(signature));
            }
        }
        meta = meta->superClass();
    } while(meta);

    QDBusMessage reply = message.createReply();
    reply << QVariant(signal_list);
    QDBusConnection::sessionBus().send(reply);
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

