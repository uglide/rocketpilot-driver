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

    virtual NodeIntrospectionData GetIntrospectionData() const;
    virtual qint64 GetObjectId() const;

    void AddChild(QObject* child);

    virtual std::string GetName() const;
    virtual std::string GetPath() const;
    virtual xpathselect::NodeVector Children() const;
private:
    QCoreApplication* application_;
    QList<QObject*> children_;
};

#endif // ROOTNODE_H
