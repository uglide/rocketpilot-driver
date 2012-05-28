#ifndef DBUS_OBJECT_H
#define DBUS_OBJECT_H

#include <QObject>
#include <QVariantMap>


class DBusObject : public QObject
{
Q_OBJECT
public:

public Q_SLOTS:
    QList<QVariant> GetState(const QString &piece);

};

#endif
