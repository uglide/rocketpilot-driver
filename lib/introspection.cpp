#include "introspection.h"

#include <QObject>
#include <QMetaProperty>
#include <QDebug>
#include <QVariant>
#include <QMap>

void PopulateMapWithProperties(QVariantMap &map, const QObject* const obj);
bool IsValidDBusType(QVariant::Type t);
QObjectList GetObjectChildren(const QObject* const obj);


QVariantMap Introspect(const QObject* obj)
{
    QVariantMap ret;

    PopulateMapWithProperties(ret, obj);

    QList<QVariant> children;
    foreach(QObject* child, GetObjectChildren(obj))
    {
//        QList<QVariant> child_struct;
//        child_struct << QVariant(child->metaObject()->className())
//            << QVariant(Introspect(child));
        children.append(QVariant(child->metaObject()->className()));
    }
    if (children.length())
        ret["Children"] = QVariant(children);

    return ret;
}


void PopulateMapWithProperties(QVariantMap &map, const QObject* const obj)
{
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
        map[prop.name()] = prop.read(obj);
    }
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

QObjectList GetObjectChildren(const QObject* const obj)
{
    QObjectList children = obj->children();
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->parent() != obj)
            it = children.erase(it);
    }
    return children;
}
