#include "rootnode.h"
#include "introspection.h"

#include <QObject>
#include <QCoreApplication>
#include <QStringList>

RootNode::RootNode(QCoreApplication* application)
    : QtNode(application, std::string())
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

    data.state["Children"] = child_names;
    data.state["id"] = GetObjectId();
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

// TODO: We can probably delete this method, since QtNode should handle it just fine.
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
