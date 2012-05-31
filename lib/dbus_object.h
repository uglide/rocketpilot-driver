/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

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
