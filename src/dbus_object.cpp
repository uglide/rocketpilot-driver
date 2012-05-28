#include "dbus_object.h"

#include <QList>
#include <QVariantMap>
#include <QDebug>

QList<QVariant> DBusObject::GetState(const QString &piece)
{
    QList<QVariant> state;
    qDebug("Inside GetState()");
    QVariantMap m;
    m["some_property"] = QString("Some Value");
    state.append(QVariant(m));
    qDebug() << "Returning:" << state;
    return state;
}
