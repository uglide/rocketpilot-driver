#include "rootnode.h"
#include "introspection.h"

#include <QObject>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

RootNode::RootNode(QCoreApplication* application)
    : QtNode(application)
    , application_(application)
{
}


NodeIntrospectionData RootNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetNodeProperties(application_);
    QStringList child_names;
    foreach(QObject* child, children_)
    {
        child_names.append(child->metaObject()->className());
    }

    data.state["Children"] = PackProperty(child_names);
    data.state["id"] = PackProperty(GetObjectId());
    return data;
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

std::string RootNode::GetPath() const
{
    return "/" + GetName();
}

xpathselect::NodeVector RootNode::Children() const
{
    xpathselect::NodeVector children;
    foreach(QObject* child, children_)
        children.push_back(std::make_shared<QtNode>(child, shared_from_this()));
    return children;
}
