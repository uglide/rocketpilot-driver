#include "dbus_object.h"

#include <QList>
#include <QVariantMap>
#include <QDebug>

QList<QVariantMap> DBusObject::GetState(const QString &piece)
{
    QList<QVariantMap> state;
    qDebug("Inside GetState()");
    QVariantMap m;
    m["some_property"] = QString("Some Value");
    state.append(m);
    qDebug() << "Returning:" << state;
    return state;
}
