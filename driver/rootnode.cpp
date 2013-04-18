#include "rootnode.h"

#include <QObject>
#include <QCoreApplication>
#include <QStringList>

RootNode::RootNode(QCoreApplication* application)
    : QtNode(application, std::string())
    , application_(application)
{
}

QVariant RootNode::IntrospectNode() const
{
    // return must be (name, state_map)
    QString object_name = QString::fromStdString(GetName());
    QStringList child_names;
    foreach(QObject* child, children_)
    {
        child_names.append(child->metaObject()->className());
    }

    QVariantMap object_properties;
    object_properties["Children"] = child_names;
    object_properties["id"] = GetObjectId();
    QList<QVariant> object_tuple = { QVariant(object_name), QVariant(object_properties) };
    return QVariant(object_tuple);
}

qint64 RootNode::GetObjectId() const
{
    return 1;
}

void RootNode::AddChild(QObject* child)
{
    children_.append(child);
}

std::string RootNode::GetName() const
{
    QString appName = application_->applicationName().remove(' ').remove('.');
    return appName.isEmpty() ? "Root" : appName.toStdString();
}

bool RootNode::MatchProperty(const std::string& name, const std::string& value) const
{
    if (name == "id")
        return QString::fromStdString(value).toLongLong() == GetObjectId();

    return false;
}

xpathselect::NodeList RootNode::Children() const
{
    xpathselect::NodeList children;
    foreach(QObject* child, children_)
        children.push_back(std::make_shared<QtNode>(child, GetPath()));
    return children;
}
