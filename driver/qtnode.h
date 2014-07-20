#ifndef QTNODE_H
#define QTNODE_H

#include <cstdint>
#include <QVariant>
#include <QDBusArgument>
#include <xpathselect/node.h>

#include <QModelIndex>

class QAbstractItemView;
class QTableWidgetItem;
class QTreeView;
class QTreeWidgetItem;

/// A simple data structure representing the state of a single node:
struct NodeIntrospectionData
{
    QString object_path;
    QVariantMap state;
};

Q_DECLARE_METATYPE(NodeIntrospectionData);
Q_DECLARE_METATYPE(QList<NodeIntrospectionData>);

QDBusArgument &operator<<(QDBusArgument &argument, NodeIntrospectionData const& node_data);
const QDBusArgument &operator>>(QDBusArgument const& argument, NodeIntrospectionData &node_data);

// Interface for Introspecting an object to query it's details.
class DBusNode : public xpathselect::Node
{
public:
    typedef std::shared_ptr<const DBusNode> Ptr;

    DBusNode() {}
    virtual ~DBusNode() {}

    virtual NodeIntrospectionData GetIntrospectionData() const=0;
};

/// Specialist class for all QObject object nodes.
/// This will cover a majority of what we use and we will only need to break
/// out to specialist classes for a couple of minor edge-cases (i.e. QModelIndex)
///
/// QObjectNode wraps a single QObject pointer. It derives from
/// xpathselect::Node (DBusNode) and, like that class, is designed to be
/// allocated on the heap and stored in a std::shared_ptr.
class QObjectNode : public DBusNode, public std::enable_shared_from_this<QObjectNode>
{
public:
    typedef std::shared_ptr<const QObjectNode> Ptr;

    QObjectNode(QObject* object, DBusNode::Ptr parent);
    explicit QObjectNode(QObject* object);

    QObject* getWrappedObject() const;

    // DBusNode
    virtual NodeIntrospectionData GetIntrospectionData() const;

    // xpathselect::Node
    xpathselect::Node::Ptr GetParent() const;
    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual int32_t GetId() const;
    virtual bool MatchStringProperty(std::string const& name, std::string const& value) const;
    virtual bool MatchIntegerProperty(std::string const& name, int32_t value) const;
    virtual bool MatchBooleanProperty(std::string const& name, bool value) const;
    virtual xpathselect::NodeVector Children() const;

private:
    QObject *object_;
    std::string full_path_;
    DBusNode::Ptr parent_;
};

class QModelIndexNode : public DBusNode, public std::enable_shared_from_this<QModelIndexNode>
{
public:
    QModelIndexNode(QModelIndex index, QAbstractItemView* parent_view, DBusNode::Ptr parent);
    explicit QModelIndexNode(QModelIndex index, QAbstractItemView* parent_view);

    // DBusNode
    virtual NodeIntrospectionData GetIntrospectionData() const;

    // xpathselect::Node
    xpathselect::Node::Ptr GetParent() const;
    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual int32_t GetId() const;
    virtual bool MatchStringProperty(std::string const& name, std::string const& value) const;
    virtual bool MatchIntegerProperty(std::string const& name, int32_t value) const;
    virtual bool MatchBooleanProperty(std::string const& name, bool value) const;
    virtual xpathselect::NodeVector Children() const;

private:
    QVariantMap GetProperties() const;

    QModelIndex index_;
    QAbstractItemView* parent_view_;
    std::string full_path_;
    DBusNode::Ptr parent_;
};

class QTableWidgetItemNode : public DBusNode, public std::enable_shared_from_this<QTableWidgetItemNode>
{
public:
    QTableWidgetItemNode(QTableWidgetItem *item, DBusNode::Ptr parent);
    explicit QTableWidgetItemNode(QTableWidgetItem *item);

    // DBusNode
    virtual NodeIntrospectionData GetIntrospectionData() const;

    // xpathselect::Node
    xpathselect::Node::Ptr GetParent() const;
    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual int32_t GetId() const;
    virtual bool MatchStringProperty(std::string const& name, std::string const& value) const;
    virtual bool MatchIntegerProperty(std::string const& name, int32_t value) const;
    virtual bool MatchBooleanProperty(std::string const& name, bool value) const;
    virtual xpathselect::NodeVector Children() const;

private:
    QVariantMap GetProperties() const;

    QTableWidgetItem *item_;
    std::string full_path_;
    DBusNode::Ptr parent_;
};

class QTreeWidgetItemNode : public DBusNode, public std::enable_shared_from_this<QTreeWidgetItemNode>
{
public:
    QTreeWidgetItemNode(QTreeWidgetItem *item, DBusNode::Ptr parent);
    explicit QTreeWidgetItemNode(QTreeWidgetItem *item);

    // DBusNode
    virtual NodeIntrospectionData GetIntrospectionData() const;

    // xpathselect::Node
    xpathselect::Node::Ptr GetParent() const;
    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual int32_t GetId() const;
    virtual bool MatchStringProperty(std::string const& name, std::string const& value) const;
    virtual bool MatchIntegerProperty(std::string const& name, int32_t value) const;
    virtual bool MatchBooleanProperty(std::string const& name, bool value) const;
    virtual xpathselect::NodeVector Children() const;

private:
    QVariantMap GetProperties() const;

    QTreeWidgetItem *item_;
    std::string full_path_;
    DBusNode::Ptr parent_;
};

#endif // QTNODE_H
