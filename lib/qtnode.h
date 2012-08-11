#ifndef QTNODE_H
#define QTNODE_H

#include <QVariant>
#include <node.h>

/// Base class for all Qt-based object nodes.
class QtNode: public xpathselect::Node
{
public:
    typedef std::shared_ptr<QtNode> Ptr;

    QtNode(QObject* object);
    virtual QVariant IntrospectNode() const;

    virtual std::string GetName() const;
    virtual bool MatchProperty(const std::string& name, const std::string& value) const;
    virtual xpathselect::NodeList Children() const;
private:
    QObject *object_;
};

#endif // QTNODE_H
