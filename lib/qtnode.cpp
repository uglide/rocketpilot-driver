#include "qtnode.h"

#include "introspection.h"


QtNode::QtNode(QObject *obj)
    : object_(obj)
{
}

QVariant QtNode::IntrospectNode() const
{
    // return must be (name, state_map)
    QString object_name = QString::fromStdString(GetName());
    QVariantMap object_properties = GetNodeProperties(object_);
    QList<QVariant> object_tuple = { QVariant(object_name), QVariant(object_properties) };
    return QVariant(object_tuple);
}

std::string QtNode::GetName() const
{
    QString name = object_->metaObject()->className();
    return name.toStdString();
}

bool QtNode::MatchProperty(const std::string& name, const std::string& value) const
{
    ///TODO

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
