#include "dbus_adaptor_qt.h"

#include <QDebug>

AutopilotQtSpecificAdaptor::AutopilotQtSpecificAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
}

void AutopilotQtSpecificAdaptor::RegisterSignalInterest(int object_id, QString signal_name)
{
    QMetaObject::invokeMethod(
                parent(),
                "RegisterSignalInterest",
                Qt::QueuedConnection,
                Q_ARG(int, object_id),
                Q_ARG(QString, signal_name)
                );
}

void AutopilotQtSpecificAdaptor::GetSignalEmissions(int object_id, QString signal_name, const QDBusMessage &message)
{
    message.setDelayedReply(true);
    QMetaObject::invokeMethod(
                parent(),
                "GetSignalEmissions",
                Qt::QueuedConnection,
                Q_ARG(int, object_id),
                Q_ARG(QString, signal_name),
                Q_ARG(QDBusMessage, message)
                );
}

void AutopilotQtSpecificAdaptor::ListSignals(int object_id, const QDBusMessage& message)
{
    message.setDelayedReply(true);
    QMetaObject::invokeMethod(
                parent(),
                "ListSignals",
                Qt::QueuedConnection,
                Q_ARG(int, object_id),
                Q_ARG(QDBusMessage, message)
                );
}

void AutopilotQtSpecificAdaptor::ListMethods(int object_id, const QDBusMessage& message)
{
    message.setDelayedReply(true);
    QMetaObject::invokeMethod(
                parent(),
                "ListMethods",
                Qt::QueuedConnection,
                Q_ARG(int, object_id),
                Q_ARG(QDBusMessage, message)
                );
}

void AutopilotQtSpecificAdaptor::InvokeMethod(int object_id, QString method_name, QVariantList args, const QDBusMessage &message)
{
    QMetaObject::invokeMethod(
                parent(),
                "InvokeMethod",
                Qt::QueuedConnection,
                Q_ARG(int, object_id),
                Q_ARG(QString, method_name),
                Q_ARG(QVariantList, args),
                Q_ARG(QDBusMessage, message)
                );
}
