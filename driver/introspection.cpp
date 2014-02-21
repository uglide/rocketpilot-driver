/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/


#include <xpathselect/node.h>
#include <xpathselect/xpathselect.h>

#include <QDebug>

#ifdef QT5_SUPPORT
  #include <QtWidgets/QApplication>
  #include <QtWidgets/QGraphicsItem>
  #include <QtWidgets/QGraphicsScene>
  #include <QtWidgets/QGraphicsView>
  #include <QtWidgets/QWidget>
  #include <QtQuick/QQuickItem>
  #include <QtQuick/QQuickWindow>
#else
  #include <QGraphicsItem>
  #include <QGraphicsScene>
  #include <QGraphicsView>
  #include <QApplication>
  #include <QWidget>
#endif

#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QRect>
#include <QUrl>
#include <QDateTime>

#include "autopilot_types.h"
#include "introspection.h"
#include "qtnode.h"
#include "rootnode.h"


QVariant IntrospectNode(QObject* obj);
QString GetNodeName(QObject* obj);
QStringList GetNodeChildNames(QObject* obj);

QVariant GetGlobalRect(QObject* obj);
QVariant GetChildrenNames(QObject* obj);

QList<NodeIntrospectionData> Introspect(QString const& query_string)
{
    QList<NodeIntrospectionData> state;
    QList<QtNode::Ptr> node_list = GetNodesThatMatchQuery(query_string);
    foreach (QtNode::Ptr obj, node_list)
    {
        state.append(obj->GetIntrospectionData());
    }

    return state;
}


QList<QtNode::Ptr> GetNodesThatMatchQuery(QString const& query_string)
{
#ifdef QT5_SUPPORT
    std::shared_ptr<RootNode> root = std::make_shared<RootNode>(QApplication::instance());

    // Add all QWidget top level widgets
    foreach (const QWidget *widget, QApplication::topLevelWidgets())
    {
        root->AddChild((QObject*) widget);
    }
    // Add all QML top level Windows
    foreach (QWindow *widget, QGuiApplication::allWindows())
    {
        root->AddChild((QObject*) widget);
    }
#else
    std::shared_ptr<RootNode> root = std::make_shared<RootNode>(QApplication::instance());
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        root->AddChild((QObject*) widget);
    }
#endif
    QList<QtNode::Ptr> node_list;

    xpathselect::NodeVector list = xpathselect::SelectNodes(root, query_string.toStdString());
    for (auto node : list)
    {
        // node may be our root node wrapper *or* an ordinary qobject wrapper
        auto object_ptr = std::static_pointer_cast<const QtNode>(node);
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
    do
    {
        for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
        {
            QMetaProperty prop = meta->property(i);
            if (!prop.isValid())
            {
                qDebug() << "Property at index " << i << " is not valid!";
                continue;
            }
            QVariant object_property = PackProperty(prop.read(obj));
            if (! object_property.isValid())
                continue;
            if (!object_properties.contains(prop.name())) {
                object_properties[prop.name()] = object_property;
            }
        }
        foreach(const QByteArray &dynamicPropertyName, obj->dynamicPropertyNames()) {
            QVariant dynamicPropertyValue = obj->property(dynamicPropertyName);

            QVariant object_property = PackProperty(dynamicPropertyValue);
            if (! object_property.isValid())
                continue;
            object_properties[dynamicPropertyName] = object_property;
        }

        meta = meta->superClass();
    } while(meta);

    QVariant global_rect_property = GetNodeProperty(obj, "globalRect");
    if(global_rect_property.isValid())
        object_properties["globalRect"] = global_rect_property;

    // add the 'Children' pseudo-property:
    QVariant children_property = GetNodeProperty(obj, "Children");
    if(children_property.isValid())
        object_properties["Children"] = children_property;

    return object_properties;
}

QVariant GetNodeProperty(QObject* obj, const std::string& property_name)
{
    if(property_name == "globalRect")
        return GetGlobalRect(obj);

    if(property_name == "Children")
    {
        QStringList children = GetNodeChildNames(obj);
        if (!children.empty())
            return PackProperty(children);
        else
            return QVariant(QVariant::Invalid);
    }

    QVariant dynamic_property = obj->property(property_name.c_str());
    if (dynamic_property.isValid())
    {
        return PackProperty(dynamic_property);
    }
    else
    {
        const QMetaObject* meta = obj->metaObject();
        int property_index = meta->indexOfProperty(property_name.c_str());
        if(property_index != -1)
        {
            QMetaProperty prop = meta->property(property_index);
            if(prop.isValid())
                return PackProperty(prop.read(obj));
            else
                qDebug() << "Property " << QString::fromStdString(property_name)
                         << " is not valid.";
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GetChildrenNames(QObject* obj)
{
    QStringList children = GetNodeChildNames(obj);
    if (!children.empty())
        return PackProperty(children);
    else
        return QVariant(QVariant::Invalid);
}

QVariant GetGlobalRect(QObject* obj)
{
    // Add any custom properties we need to the given QObject.
    // Add GlobalRect support for QWidget-derived classes
    QWidget *w = qobject_cast<QWidget*>(obj);
    if (w)
    {
        QRect r = w->rect();
        r = QRect(w->mapToGlobal(r.topLeft()), r.size());
        return PackProperty(r);
    }
    // ...and support for QGraphicsItem-derived classes.
    else if (QGraphicsItem *i = qobject_cast<QGraphicsItem*>(obj))
    {
        // need to get the view that this item is in. Should only be one. If there's
        // more than one, we're in trouble.
        QGraphicsView *view = i->scene()->views().last();
        QRectF bounding_rect = i->boundingRect();
        bounding_rect = i->mapRectToScene(bounding_rect);
        QRect scene_rect = view->mapFromScene(bounding_rect).boundingRect();
        QRect global_rect = QRect(
                    view->mapToGlobal(scene_rect.topLeft()),
                    scene_rect.size());
        return PackProperty(global_rect);
    }
#ifdef QT5_SUPPORT
    // ... and support for QQuickItems (aka. Qt5 Declarative items)
    else if (QQuickItem *i = qobject_cast<QQuickItem*>(obj))
    {
        QQuickWindow *view = i->window();
        if(view) {
            QRectF bounding_rect = i->boundingRect();
            bounding_rect = i->mapRectToScene(bounding_rect);
            QRect global_rect = QRect(
                view->mapToGlobal(bounding_rect.toRect().topLeft()), bounding_rect.size().toSize()
                );

            return PackProperty(global_rect);
        }
    }
#endif

    // Default to returning invalid QVariant
    return QVariant(QVariant::Invalid);
}

QVariant PackProperty(QVariant const& prop)
{
    switch (prop.type())
    {
    case QVariant::Int:
    case QVariant::Bool:
    case QVariant::String:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::StringList:
    case QVariant::Double:
    {
        return QList<QVariant> {
            QVariant(TYPE_PLAIN),
            prop
        };
    }

    case QVariant::ByteArray:
    {
        return QList<QVariant> {
            QVariant(TYPE_PLAIN),
            QVariant(QString(qvariant_cast<QByteArray>(prop)))
        };
    }

    case QVariant::Point:
    {
        QPoint p = qvariant_cast<QPoint>(prop);
        return QList<QVariant> {
            QVariant(TYPE_POINT),
            QVariant(p.x()),
            QVariant(p.y())
        };
    }

    case QVariant::Rect:
    {
        QRect r = qvariant_cast<QRect>(prop);
        return QList<QVariant> {
            QVariant(TYPE_RECT),
            QVariant(r.x()),
            QVariant(r.y()),
            QVariant(r.width()),
            QVariant(r.height()) };
    }

    case QVariant::Size:
    {
        QSize s = qvariant_cast<QSize>(prop);
        return QList<QVariant> {
            QVariant(TYPE_SIZE),
            QVariant(s.width()),
            QVariant(s.height())
        };
    }

    case QVariant::Color:
    {
        QColor color = qvariant_cast<QColor>(prop).toRgb();
        return QList<QVariant> {
            QVariant(TYPE_COLOR),
            QVariant(color.red()),
            QVariant(color.green()),
            QVariant(color.blue()),
            QVariant(color.alpha())
        };
    }

    case QVariant::Url:
    {
        return QList<QVariant> {
            QVariant(TYPE_PLAIN),
            QVariant(prop.toUrl().toString())
        };
    }

    // Depending on the architecture, floating points might be of type QMetaType::Float instead of QVariant::Double
    // QDBus however, can only carry QVariant types, so lets convert it to QVariant::Double
    case QMetaType::Float:
    {
        return QList<QVariant> {
            QVariant(TYPE_PLAIN),
            QVariant(prop.toDouble())
        };
    }

    case QVariant::Date:
    case QVariant::DateTime:
    {
        return QList<QVariant> {
            QVariant(TYPE_DATETIME),
            QVariant(prop.toDateTime().toTime_t())
        };
    }

    case QVariant::Time:
    {
        QTime t = qvariant_cast<QTime>(prop);
        return QList<QVariant> {
            QVariant(TYPE_TIME),
            QVariant(t.hour()),
            QVariant(t.minute()),
            QVariant(t.second()),
            QVariant(t.msec())
        };
    }

    default:
    {
        return QVariant(); // unsupported type, will not be sent to the client.
    }
    }
}


QStringList GetNodeChildNames(QObject* obj)
{
    QStringList child_names;
    foreach (QObject *child, obj->children())
    {
        if (child->parent() == obj) {
            child_names.append(GetNodeName(child));
        }
    }
#ifdef QT5_SUPPORT
    // In case of a QQuickWindow, add the main contentItem()
    if (QQuickWindow *window = qobject_cast<QQuickWindow*>(obj)) {
        child_names.append(GetNodeName(window->contentItem()));
    }
    // In case of QQuickItems include also childItems(), not only children().
    if (QQuickItem *item = qobject_cast<QQuickItem*>(obj)) {
        foreach (QObject *child, item->childItems()) {
            child_names.append(GetNodeName(child));
        }
    }
#endif
    return child_names;
}
