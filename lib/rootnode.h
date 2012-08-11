#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "qtnode.h"

#include <QList>
class QCoreApplication;
class QObject;


class RootNode: public QtNode
{
public:
    RootNode(QCoreApplication* application);

    virtual QVariant IntrospectNode() const;
    virtual qint64 GetObjectId() const;

    void AddChild(QObject* child);

    virtual std::string GetName() const;
    virtual bool MatchProperty(const std::string& name, const std::string& value) const;
    virtual xpathselect::NodeList Children() const;
private:
    QCoreApplication* application_;
    QList<QObject*> children_;
};

#endif // ROOTNODE_H
