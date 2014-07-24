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
void AddCustomProperties(QObject* obj, QVariantMap& properties);

QList<NodeIntrospectionData> Introspect(QString const& query_string)
{
    QList<NodeIntrospectionData> state;
    QList<DBusNode::Ptr> node_list = GetNodesThatMatchQuery(query_string);
    foreach (DBusNode::Ptr obj, node_list)
    {
        state.append(obj->GetIntrospectionData());
    }

    return state;

}


QList<DBusNode::Ptr> GetNodesThatMatchQuery(QString const& query_string)
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
    QList<DBusNode::Ptr> node_list;

    xpathselect::NodeVector list = xpathselect::SelectNodes(root, query_string.toStdString());
    for (auto node : list)
    {
        // node may be our root node wrapper *or* an ordinary qobject wrapper
        auto object_ptr = std::static_pointer_cast<const DBusNode>(node);
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
                qDebug() << "Property at index" << i << "Is not valid!";
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

    AddCustomProperties(obj, object_properties);

    // add the 'Children' pseudo-property:
    QStringList children = GetNodeChildNames(obj);
    if (!children.empty())
        object_properties["Children"] = PackProperty(children);

    return object_properties;
}


void AddCustomProperties(QObject* obj, QVariantMap &properties)
{
    // Add any custom properties we need to the given QObject.
    // Add GlobalRect support for QWidget-derived classes
    QWidget *w = qobject_cast<QWidget*>(obj);
    if (w)
    {
        QRect r = w->rect();
        r = QRect(w->mapToGlobal(r.topLeft()), r.size());
        properties["globalRect"] = PackProperty(r);
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
        properties["globalRect"] = PackProperty(global_rect);
    }
#ifdef QT5_SUPPORT
    // ... and support for QQuickItems (aka. Qt5 Declarative items)
    else if (QQuickItem *i = qobject_cast<QQuickItem*>(obj))
    {
        QQuickWindow *view = i->window();
        QRectF bounding_rect = i->boundingRect();
        bounding_rect = i->mapRectToScene(bounding_rect);
        QRect global_rect = QRect(view->mapToGlobal(bounding_rect.toRect().topLeft()), bounding_rect.size().toSize());
        properties["globalRect"] = PackProperty(global_rect);
    }
#endif
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
