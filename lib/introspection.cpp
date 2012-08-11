/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/


#include <node.h>
#include <xpathselect.h>

#include <QApplication>
#include <QDebug>
#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QStringList>
#include <QVariant>

#include "introspection.h"
#include "qtnode.h"
#include "rootnode.h"

QList<QtNode::Ptr> GetNodesThatMatchQuery(QString const& query_string);
QVariant IntrospectNode(QObject* obj);
QString GetNodeName(QObject* obj);
QStringList GetNodeChildNames(QObject* obj);

QList<QVariant> Introspect(QString const& query_string)
{
    QList<QVariant> state;
    QList<QtNode::Ptr> node_list = GetNodesThatMatchQuery(query_string);
    foreach (QtNode::Ptr obj, node_list)
    {
        state.append(obj->IntrospectNode());
    }

    return state;

}


QList<QtNode::Ptr> GetNodesThatMatchQuery(QString const& query_string)
{
    std::shared_ptr<RootNode> root = std::make_shared<RootNode>(QApplication::instance());

    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        root->AddChild((QObject*) widget);
    }

    QList<QtNode::Ptr> node_list;

    xpathselect::NodeList list = xpathselect::SelectNodes(root, query_string.toStdString());
    qDebug() << "XPathSelect library returned" << list.size() << "items.";
    for (auto node : list)
    {
        // node may be our root node wrapper *or* an ordinary qobject wrapper
        auto object_ptr = std::static_pointer_cast<QtNode>(node);
        if (object_ptr)
        {
            node_list.append(object_ptr);
        }

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
