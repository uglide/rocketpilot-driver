#include "qtnode.h"

#include "introspection.h"

#include <QDebug>

#ifdef QT5_SUPPORT
  #include <QtWidgets/QGraphicsScene>
  #include <QtWidgets/QGraphicsObject>
  #include <QtQml/QQmlEngine>
  #include <QtQml/QQmlContext>
  #include <QtQuick/QQuickView>
  #include <QtQuick/QQuickItem>
#else
  #include <QGraphicsScene>
  #include <QGraphicsObject>
#endif
#include <QDBusArgument>

// Marshall the NodeIntrospectionData data into a D-Bus argument
 QDBusArgument &operator<<(QDBusArgument &argument, const NodeIntrospectionData &node_data)
 {
     argument.beginStructure();
     argument << node_data.object_path << node_data.state;
     argument.endStructure();
     return argument;
 }

 // Retrieve the NodeIntrospectionData data from the D-Bus argument
 const QDBusArgument &operator>>(const QDBusArgument &argument, NodeIntrospectionData &node_data)
 {
     argument.beginStructure();
     argument >> node_data.object_path >> node_data.state;
     argument.endStructure();
     return argument;
 }

const QByteArray AP_ID_NAME("_autopilot_id");

QtNode::QtNode(QObject *obj, std::string const& parent_path)
    : object_(obj)
{
    full_path_ = parent_path + "/" + GetName();
}

QObject* QtNode::getWrappedObject() const
{
    return object_;
}

NodeIntrospectionData QtNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetNodeProperties(object_);
    data.state["id"] = PackProperty(GetObjectId());
    return data;
}

qint64 QtNode::GetObjectId() const
{
    // Note: This starts at 1 for a reason - 1 is reserved for the pseudo root node, and
    // so must never be allocated to a regular object.
    static qint64 next_id=1;

    QList<QByteArray> property_names = object_->dynamicPropertyNames();
    if (!property_names.contains(AP_ID_NAME))
        object_->setProperty(AP_ID_NAME, QVariant(++next_id));
    return object_->property(AP_ID_NAME).toLongLong();

}

std::string QtNode::GetName() const
{
    QString name = object_->metaObject()->className();

    // QML type names get mangled by Qt - they get _QML_N or _QMLTYPE_N appended.
    //
    if (name.contains('_'))
        name = name.split('_').front();
    return name.toStdString();
}

std::string QtNode::GetPath() const
{
    return full_path_;
}

bool QtNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(QString::fromStdString(value));
    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

bool QtNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetObjectId();

    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(value);
    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

bool QtNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(value);

    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

xpathselect::NodeList QtNode::Children() const
{
    xpathselect::NodeList children;

#ifdef QT5_SUPPORT
    // Qt5's hierarchy for QML has changed a bit:
    // - On top there's a QQuickView which holds all the QQuick items
    // - QQuickItems don't always follow the QObject type hierarchy (e.g. QQuickListView does not), therefore we use the QQuickItem's childItems()
    // - In case it is not a QQuickItem, fall back to the standard QObject hierarchy

    QQuickView *view = qobject_cast<QQuickView*>(object_);
    if (view && view->rootObject() != 0) {
        children.push_back(std::make_shared<QtNode>(view->rootObject(), GetPath()));
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(object_);
    if (item) {
        foreach (QQuickItem *childItem, item->childItems()) {
            if (childItem->parentItem() == item) {
                children.push_back(std::make_shared<QtNode>(childItem, GetPath()));
            }
        }
    } else {
        foreach (QObject *child, object_->children())
        {
            if (child->parent() == object_)
                children.push_back(std::make_shared<QtNode>(child, GetPath()));
        }
    }

#else
    foreach (QObject *child, object_->children())
    {
        if (child->parent() == object_)
            children.push_back(std::make_shared<QtNode>(child, GetPath()));
    }

    // If our wrapped object is a QGraphicsScene, we need to explicitly grab any child graphics
    // items that are derived from QObjects. Declarative UIs use this idiom, so this need to be
    // done to support QML applications.
    QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(object_);
    if (scene)
    {
        QList<QGraphicsItem*> child_items = scene->items();
        foreach(QGraphicsItem* item, child_items)
        {
            QGraphicsObject *obj = item->toGraphicsObject();
            if (obj && ! obj->parent())
                children.push_back(std::make_shared<QtNode>(obj, GetPath()));
        }
    }
#endif

    return children;
}
