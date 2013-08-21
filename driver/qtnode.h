#ifndef QTNODE_H
#define QTNODE_H

#include <cstdint>
#include <QVariant>
#include <QDBusArgument>
#include <xpathselect/node.h>

/// A simple data structure representing the state of a single node:
struct NodeIntrospectionData
{
    QString object_path;
    QVariantMap state;
};

Q_DECLARE_METATYPE(NodeIntrospectionData);
Q_DECLARE_METATYPE(QList<NodeIntrospectionData>);

QDBusArgument &operator<<(QDBusArgument &argument, const NodeIntrospectionData &node_data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NodeIntrospectionData &node_data);

/// Base class for all Qt-based object nodes.
///
/// QtNode wraps a single QObject pointer. It derives from xpathselect::Node and,
/// like that class, is designed to be allocated on the heap and stored in a
/// std::shared_ptr.
class QtNode: public xpathselect::Node
{
public:
    typedef std::shared_ptr<QtNode> Ptr;


    QtNode(QObject* object, std::string const& parent_path);

    QObject* getWrappedObject() const;

    virtual NodeIntrospectionData GetIntrospectionData() const;

    virtual qint64 GetObjectId() const;

    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual bool MatchStringProperty(const std::string& name, const std::string& value) const;
    virtual bool MatchIntegerProperty(const std::string& name, int32_t value) const;
    virtual bool MatchBooleanProperty(const std::string& name, bool value) const;
    virtual xpathselect::NodeList Children() const;
private:
    QObject *object_;
    std::string full_path_;
};

#endif // QTNODE_H
