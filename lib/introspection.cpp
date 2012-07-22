/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "introspection.h"

#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QStringList>
#include <QVariant>

QObjectList GetNodesThatMatchQuery(QString const& query_string);
QVariant IntrospectNode(QObject* obj);
QString GetNodeName(QObject* obj);
QVariantMap GetNodeProperties(QObject* obj);
bool IsValidDBusType(QVariant::Type t);
QStringList GetNodeChildNames(QObject* obj);


QList<QVariant> Introspect(QString const& query_string)
{
    QList<QVariant> state;
    QObjectList node_list = GetNodesThatMatchQuery(query_string);
    foreach (QObject* obj, node_list)
    {
        state.append(IntrospectNode(obj));
    }

    return state;

}


QObjectList GetNodesThatMatchQuery(QString const& query_string)
{
    QObjectList node_list;
    ///\TODO - populate with only the nodes that match the query string.
    QObject *obj = QApplication::instance();
    node_list.append(obj);

    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
    node_list.append((QObject*) widget);
    }

    return node_list;
}


QVariant IntrospectNode(QObject* obj)
{
    // return must be (name, state_map)
    QString object_name = GetNodeName(obj);
    QVariantMap object_properties = GetNodeProperties(obj);
    QList<QVariant> object_tuple = { QVariant(object_name), QVariant(object_properties) };
    return QVariant(object_tuple);
}


QString GetNodeName(QObject* obj)
{
    return obj->metaObject()->className();
}


QVariantMap GetNodeProperties(QObject* obj)
{
    QVariantMap object_properties;
    const QMetaObject* meta = obj->metaObject();
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
        QMetaProperty prop = meta->property(i);
        if (!prop.isValid())
        {
            qDebug() << "Property at index" << i << "Is not valid!";
            continue;
        }
        if (! IsValidDBusType(prop.type()))
            continue;
        object_properties[prop.name()] = prop.read(obj);
    }
    QStringList children = GetNodeChildNames(obj);
    if (!children.empty())
        object_properties["Children"] = children;
    return object_properties;
}


bool IsValidDBusType(QVariant::Type t)
{
    switch (t)
    {
        case QVariant::Int:
        case QVariant::Bool:
        case QVariant::String:
        case QVariant::UInt:
        case QVariant::ULongLong:
        {
            return true;
        }

        default:
        {
            return false;
        }
    }
}


QStringList GetNodeChildNames(QObject* obj)
{
    QStringList child_names;
    foreach (QObject *child, obj->children())
    {
        if (child->parent() == obj)
            child_names.append(GetNodeName(child));
    }
    return child_names;
}
