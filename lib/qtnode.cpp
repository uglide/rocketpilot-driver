#include "qtnode.h"

#include "introspection.h"

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
    ///TODO
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
    return children;
}
