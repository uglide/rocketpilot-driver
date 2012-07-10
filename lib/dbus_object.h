/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#ifndef DBUS_OBJECT_H
#define DBUS_OBJECT_H

#include <QObject>
#include <QPair>
#include <QQueue>
#include <QDBusMessage>
#include <QTimer>


class DBusObject : public QObject
{
Q_OBJECT
public:
    DBusObject(QObject* parent=nullptr);

public slots:
    void GetState(const QString &piece, const QDBusMessage& msg);

private slots:
    void ProcessQuery();

private:
    typedef QPair<QString, QDBusMessage> Query;
    QQueue<Query> _queries;
};

#endif
