#ifndef DBUS_OBJECT_H
#define DBUS_OBJECT_H

#include <QObject>
#include <QVariantMap>


class DBusObject : public QObject
{
Q_OBJECT
public:

public Q_SLOTS:
    QList<QVariantMap> GetState(const QString &piece);

};

#endif
