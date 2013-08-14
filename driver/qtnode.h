#ifndef QTNODE_H
#define QTNODE_H

#include <QVariant>
#include <xpathselect/node.h>

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

    virtual QVariant IntrospectNode() const;
    virtual qint64 GetObjectId() const;

    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual bool MatchStringProperty(const std::string& name, const std::string& value) const;
    virtual bool MatchIntegerProperty(const std::string& name, int value) const;
    virtual bool MatchBooleanProperty(const std::string& name, bool value) const;
    virtual xpathselect::NodeList Children() const;
private:
    QObject *object_;
    std::string full_path_;
};

#endif // QTNODE_H
