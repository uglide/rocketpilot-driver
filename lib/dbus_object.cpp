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

#ifdef QT5_SUPPORT
  #include <QtWidgets/QApplication>
#else
  #include <QApplication>
#endif

#include <QDBusConnection>
#include <QThread>

QtNode::Ptr GetNodeWithId(int object_id)
{
    QString query = QString("//*[id=%1]").arg(object_id);
    QList<QtNode::Ptr> objects = GetNodesThatMatchQuery(query);

    if (objects.isEmpty())
    {
        qWarning() << "No Object with with id" << object_id << "found in object tree.";
        return QtNode::Ptr();
    }

    return objects.at(0);
}

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

    QtNode::Ptr node = GetNodeWithId(object_id);
    if (! node)
        return;

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
        qDebug() << "Signal emissions" << signal_spy.data()->length() << signal_spy.data();
        for (int i = 0; i < signal_spy->length(); ++i)
        {
            QList<QVariant> signal_emission;
            foreach(const QVariant &arg, signal_spy->at(i)) {

                // We cannot marshall QObject* or QObject:
                // Marshalling a pointer through DBus makes no sense as its just an address to protected memory
                // Marshalling a QObject (without pointer) is not possible because of QObjects no-copy-nature
                if((int)arg.type() != (int)QMetaType::QObjectStar) {
                    signal_emission.append(arg);
                }
            }

            signal_emit_list.append(QVariant(signal_emission));
        }
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
    QtNode::Ptr node = GetNodeWithId(object_id);
    if (! node)
        return;

    QObject *object = node->getWrappedObject();
    const QMetaObject *meta = object->metaObject();
    QList<QVariant> signal_list;
    do
    {
        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
        {
            QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Signal)
            {
#ifdef QT5_SUPPORT
                QString signature = QString::fromLatin1(method.methodSignature());
#else
                QString signature = QString::fromLatin1(method.signature());
#endif
                signal_list.append(QVariant(signature));
            }
        }
        meta = meta->superClass();
    } while(meta);

    QDBusMessage reply = message.createReply();
    reply << QVariant(signal_list);
    QDBusConnection::sessionBus().send(reply);
}

void DBusObject::ListMethods(int object_id, const QDBusMessage &message)
{
    QtNode::Ptr node = GetNodeWithId(object_id);
    if (! node)
    {
        qWarning() << "No Object found.";
        return;
    }

    QObject *object = node->getWrappedObject();
    const QMetaObject *meta = object->metaObject();
    QList<QVariant> method_list;
    do
    {
        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
        {
            QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Slot ||
                method.methodType() == QMetaMethod::Method)
            {
#ifdef QT5_SUPPORT
                QString signature = QString::fromLatin1(method.methodSignature());
#else
                QString signature = QString::fromLatin1(method.signature());
#endif
                method_list.append(QVariant(signature));
            }
        }
        meta = meta->superClass();
    } while(meta);

    QDBusMessage reply = message.createReply();
    reply << QVariant(method_list);
    QDBusConnection::sessionBus().send(reply);
}

void DBusObject::InvokeMethod(int object_id, QString method_name, QVariantList args, const QDBusMessage &message)
{
    QtNode::Ptr node = GetNodeWithId(object_id);
    if (! node)
    {
        qWarning() << "No Object found.";
        return;
    }

    QObject *object = node->getWrappedObject();
    const QMetaObject *meta = object->metaObject();

    int method_index = -1;
    do
    {
        method_index = meta->indexOfMethod(method_name.toAscii());
        if (method_index == -1)
            meta = meta->superClass();
    } while(meta && method_index == -1);

    if (method_index == -1)
    {
        qWarning() << "Unable to find method" << method_name << "On object with id" << object_id;
        return;
    }

    QMetaMethod method = meta->method(method_index);

    qDebug() << "Method parameter names:" << method.parameterNames();
    qDebug() << "Method parameter types:" << method.parameterTypes();

#ifdef QT5_SUPPORT
    qDebug() << "Method signature:" << method.methodSignature()
             << "return type:" << method.typeName();
#else
    qDebug() << "Method signature:" << method.signature()
             << "return type:" << method.typeName();
#endif

    QVector<QGenericArgument> generic_args(10);
    QList<QByteArray> parameterTypes = method.parameterTypes();

    if (args.size() != parameterTypes.size())
    {
        qCritical() << "Method takes" << parameterTypes.size() << "Arguments, but" << args.size()
                    << "arguments were provided instead. Not calling method.";
        return;
    }

    for (int i = 0; i < args.size(); ++i)
    {
        QVariant passed_value = args.at(i);
        QByteArray passed_type_name = passed_value.typeName();
        QByteArray required_type_name = parameterTypes.at(i);
        if (passed_type_name != required_type_name)
        {
            // TODO - try and convert to correct type... if it's needed.
            qCritical() << "Argument" << i << "Is of the wrong type.";
            qCritical() << "    Expected:" << required_type_name;
            qCritical() << "    Got:" << passed_type_name;
            break;
        }
        generic_args[i] = QGenericArgument(passed_value.typeName(), passed_value.constData());
    }

    // method.invoke(...) takes between 0 and 10 parameters. Since We can't convert a QVector into
    // an argument list (like we can in Python), I'm stuck with this terrible syntax:
    bool ret = method.invoke(object,
                  generic_args.at(0),
                  generic_args.at(1),
                  generic_args.at(2),
                  generic_args.at(3),
                  generic_args.at(4),
                  generic_args.at(5),
                  generic_args.at(6),
                  generic_args.at(7),
                  generic_args.at(8),
                  generic_args.at(9));
    if (ret)
        qDebug() << "Method Invoked.";
    else
        qDebug() << "Method invocation failed.";
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

