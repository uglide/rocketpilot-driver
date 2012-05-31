/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "dbus_object.h"
#include "introspection.h"

#include <QList>
#include <QVariantMap>
#include <QDebug>
#include <QApplication>

QList<QVariant> DBusObject::GetState(const QString &piece)
{
    QList<QVariant> state;
    qDebug("Inside GetState()");

    QObject *ob = QCoreApplication::instance();
    QVariantMap m = Introspect(ob);
    state.append(QVariant(m));

    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        QVariantMap m = Introspect((QObject*) widget);
        state.append(QVariant(m));
    }
    qDebug() << "Returning:" << state;
    return state;
}
