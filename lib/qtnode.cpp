#include "qtnode.h"

#include "introspection.h"

#include <QGraphicsScene>
#include <QGraphicsObject>

const QByteArray AP_ID_NAME("_autopilot_id");

QtNode::QtNode(QObject *obj)
    : object_(obj)
{
}

QVariant QtNode::IntrospectNode() const
{
    // return must be (name, state_map)
    QString object_name = QString::fromStdString(GetName());
    QVariantMap object_properties = GetNodeProperties(object_);
    object_properties["id"] = GetObjectId();
    QList<QVariant> object_tuple = { QVariant(object_name), QVariant(object_properties) };
    return QVariant(object_tuple);
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
    return name.toStdString();
}

bool QtNode::MatchProperty(const std::string& name, const std::string& value) const
{
    if (name == "id")
        return QString::fromStdString(value).toLongLong() == GetObjectId();
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = properties[qname];
    QVariant check_value(QString::fromStdString(value));
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
    foreach (QObject *child, object_->children())
    {
        if (child->parent() == object_)
            children.push_back(std::make_shared<QtNode>(child));
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
                children.push_back(std::make_shared<QtNode>(obj));
        }
    }
    return children;
}
