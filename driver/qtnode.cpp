#include "qtnode.h"

#include "introspection.h"

#include <QDebug>

#ifdef QT5_SUPPORT
  #include <QtWidgets/QGraphicsScene>
  #include <QtWidgets/QGraphicsObject>
  #include <QtQml/QQmlEngine>
  #include <QtQml/QQmlContext>
  #include <QtQuick/QQuickView>
  #include <QtQuick/QQuickItem>
#else
  #include <QGraphicsScene>
  #include <QGraphicsObject>
#endif
#include <QDBusArgument>

#include <QAbstractProxyModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTreeView>

const QByteArray AP_ID_NAME("_autopilot_id");

void GetTableWidgetChildren(QObject* table_obj, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetTreeViewChildren(QObject* tree_obj, xpathselect::NodeVector& children, DBusNode::Ptr parent);
QStandardItemModel* AttemptGetStandardItemModel(QAbstractItemModel* target_model);

// Marshall the NodeIntrospectionData data into a D-Bus argument
 QDBusArgument &operator<<(QDBusArgument &argument, const NodeIntrospectionData &node_data)
 {
     argument.beginStructure();
     argument << node_data.object_path << node_data.state;
     argument.endStructure();
     return argument;
 }

 // Retrieve the NodeIntrospectionData data from the D-Bus argument
 const QDBusArgument &operator>>(const QDBusArgument &argument, NodeIntrospectionData &node_data)
 {
     argument.beginStructure();
     argument >> node_data.object_path >> node_data.state;
     argument.endStructure();
     return argument;
 }

void GetTableWidgetChildren(QObject *table_obj, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    QTableWidget* table = qobject_cast<QTableWidget *>(table_obj);
    if(! table) {
        qDebug() << "! Unable to cast object to QTableWidget (even though it apparently inherits from it)";
    }

    QList<QTableWidgetItem *> tablewidgetitems = table->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive);
    foreach (QTableWidgetItem *item, tablewidgetitems){
        // std::make_shared<QTableWidgetItemNode>(item, this->shared_from_this())
        children.push_back(
            std::make_shared<QTableWidgetItemNode>(item, parent)
            );
    }
}

// Attempts to cast the supplied model to a QStandardItemModel
// (checking also for proxy models).
// Returns null/0 if unable to do so.
QStandardItemModel* AttemptGetStandardItemModel(QAbstractItemModel* target_model)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel *>(target_model);
    if(!model) {
        QAbstractProxyModel* abstract_model = qobject_cast<QAbstractProxyModel*>(target_model);
        if(abstract_model) {
            model = qobject_cast<QStandardItemModel *>(abstract_model->sourceModel());
        }
    }

    return model;
}

void GetTreeViewChildren(QObject* tree_obj, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    QTreeView* tree_view = qobject_cast<QTreeView *>(tree_obj);
    if(! tree_view) {
        qDebug() << "! Unable to cast object to QTreeView (even though it apparently inherits from it)";
    }
    // Can we get a QStandardItemModel or do we need to use a QAbstractItemModel
    QStandardItemModel* standard_model = AttemptGetStandardItemModel(tree_view->model());
    if(standard_model)
    {
        for(int c=0; c < standard_model->columnCount(); ++c) {
            for(int r=0; r < standard_model->rowCount(); ++r) {
                children.push_back(
                    std::make_shared<QStandardItemNode>(
                        standard_model->item(r, c),
                        tree_view,
                        parent)
                    );
            }
        }
    }
    else
    {
        // Do the work with a QAbstractItemModel. This could probably
        // be separated out as QAbstractItemModels deal with
        // QModelIndexes etc.
    }
}

QObjectNode::QObjectNode(QObject *obj, DBusNode::Ptr parent)
: object_(obj)
, parent_(parent)
{
    std::string parent_path = parent ? parent->GetPath() : "";
    full_path_ = parent_path + "/" + GetName();
}

QObjectNode::QObjectNode(QObject* obj)
: object_(obj)
{
    full_path_ = "/" + GetName();
}

QObject* QObjectNode::getWrappedObject() const
{
    return object_;
}

NodeIntrospectionData QObjectNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetNodeProperties(object_);
    data.state["id"] = PackProperty(GetId());
    return data;
}

std::string QObjectNode::GetName() const
{
    QString name = object_->metaObject()->className();

    // QML type names get mangled by Qt - they get _QML_N or _QMLTYPE_N appended.
    //
    if (name.contains('_'))
        name = name.split('_').front();
    return name.toStdString();
}

std::string QObjectNode::GetPath() const
{
    return full_path_;
}

int32_t QObjectNode::GetId() const
{
    // Note: This method is used to assign ids to both the root node (with a QApplication object) and
    // child nodes. This used to be separate code, but now that we export QApplication properties,
    // we can use this one method everywhere.
    static int32_t next_id=0;

    QList<QByteArray> property_names = object_->dynamicPropertyNames();
    if (!property_names.contains(AP_ID_NAME))
    {
        int32_t new_id = ++next_id;
        object_->setProperty(AP_ID_NAME, QVariant(new_id));
    }
    return qvariant_cast<int32_t>(object_->property(AP_ID_NAME));
}

bool QObjectNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(QString::fromStdString(value));
    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

bool QObjectNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();

    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(value);
    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

bool QObjectNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    if (! properties.contains(qname))
        return false;

    QVariant object_value = qvariant_cast<QVariantList>(properties[qname]).at(1);
    QVariant check_value(value);

    if (check_value.canConvert(object_value.type()))
    {
        check_value.convert(object_value.type());
        return check_value == object_value;
    }

    return false;
}

xpathselect::NodeVector QObjectNode::Children() const
{
    xpathselect::NodeVector children;

    // Do special children handling if needed.
    if(object_->inherits("QTableWidget"))
    {
        GetTableWidgetChildren(object_, children, shared_from_this());
    }
    else if(object_->inherits("QTreeView"))
    {
        GetTreeViewChildren(object_, children, shared_from_this());
    }

#ifdef QT5_SUPPORT
    // Qt5's hierarchy for QML has changed a bit:
    // - On top there's a QQuickView which holds all the QQuick items
    // - QQuickItems don't always follow the QObject type hierarchy (e.g. QQuickListView does not), therefore we use the QQuickItem's childItems()
    // - In case it is not a QQuickItem, fall back to the standard QObject hierarchy

    QQuickView *view = qobject_cast<QQuickView*>(object_);
    if (view && view->rootObject() != 0) {
        children.push_back(std::make_shared<QObjectNode>(view->rootObject(), shared_from_this()));
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(object_);
    if (item) {
        foreach (QQuickItem *childItem, item->childItems()) {
            if (childItem->parentItem() == item) {
                children.push_back(std::make_shared<QObjectNode>(childItem, shared_from_this()));
            }
        }
    } else {
        foreach (QObject *child, object_->children())
        {
            if (child->parent() == object_)
                children.push_back(std::make_shared<QObjectNode>(child, shared_from_this()));
        }
    }

#else
    foreach (QObject *child, object_->children())
    {
        if (child->parent() == object_)
            children.push_back(std::make_shared<QObjectNode>(child, shared_from_this()));
    }

    // If our wrapped object is a QGraphicsScene, we need to explicitly grab any child graphics
    // items that are derived from QObjects. Declarative UIs use this idiom, so this need to be
    // done to support QML applications.
    QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(object_);
    if (scene)
    {
        QList<QGraphicsItem*> child_items = scene->items();
        foreach(QGraphicsItem* item, child_items)
        {
            QGraphicsObject *obj = item->toGraphicsObject();
            if (obj && ! obj->parent())
                children.push_back(std::make_shared<QObjectNode>(obj, shared_from_this()));
        }
    }
#endif

    return children;
}


xpathselect::Node::Ptr QObjectNode::GetParent() const
{
    return parent_;
}

// QStandardItemNode
QStandardItemNode::QStandardItemNode(QStandardItem *item, QTreeView* parent_view, DBusNode::Ptr parent)
    : item_(item)
    , parent_view_(parent_view)
    , parent_(parent)
{
    std::string parent_path = parent ? parent->GetPath() : "";
    full_path_ = parent_path + "/" + GetName();
}

QStandardItemNode::QStandardItemNode(QStandardItem *item, QTreeView* parent_view)
    : item_(item)
    , parent_view_(parent_view)
{
    full_path_ = "/" + GetName();
}

NodeIntrospectionData QStandardItemNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetProperties();
    data.state["id"] = PackProperty(GetId());
    return data;
}

QVariantMap QStandardItemNode::GetProperties() const
{
    QVariantMap properties;
    properties["text"] = PackProperty(item_->text());

    QRect rect = parent_view_->visualRect(item_->index());
    QRect global_rect(
        parent_view_->viewport()->mapToGlobal(rect.topLeft()),
        rect.size());

    properties["globalRect"] = PackProperty(global_rect);
    return properties;
}

xpathselect::Node::Ptr QStandardItemNode::GetParent() const
{
    return parent_;
}

std::string QStandardItemNode::GetName() const
{
    return "QStandardItem";
}

std::string QStandardItemNode::GetPath() const
{
    return full_path_;
}

int32_t QStandardItemNode::GetId() const
{
    return static_cast<int32_t>(reinterpret_cast<qptrdiff>(item_));
}

bool QStandardItemNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    Q_UNUSED(name);
    Q_UNUSED(value);
    return false;
}

bool QStandardItemNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();
    return false;
}

bool QStandardItemNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    Q_UNUSED(name);
    Q_UNUSED(value);
    return false;
}

xpathselect::NodeVector QStandardItemNode::Children() const
{
    xpathselect::NodeVector children;

    // Might need to flatten the model of items within the tree itself
    // as this can get confusing.
    for(int c=0; c < item_->columnCount(); ++c) {
        for(int r=0; r < item_->rowCount(); ++r) {
            QStandardItem* child_item = item_->child(r, c);
            children.push_back(
                std::make_shared<QStandardItemNode>(
                    child_item,
                    parent_view_,
                    this->shared_from_this()
                    )
                );
        }
    }

    return children;
}


// QTableWidgetItemNode
QTableWidgetItemNode::QTableWidgetItemNode(QTableWidgetItem *item, DBusNode::Ptr parent)
    : item_(item)
    , parent_(parent)
{
    std::string parent_path = parent ? parent->GetPath() : "";
    full_path_ = parent_path + "/" + GetName();
}

QTableWidgetItemNode::QTableWidgetItemNode(QTableWidgetItem *item)
    : item_(item)
{
    full_path_ = "/" + GetName();
}

NodeIntrospectionData QTableWidgetItemNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetProperties();
    data.state["id"] = PackProperty(GetId());
    return data;
}

QVariantMap QTableWidgetItemNode::GetProperties() const
{
    QVariantMap properties;

    QTableWidget* parent = item_->tableWidget();
    QRect cellrect = parent->visualItemRect(item_);
    QRect r = QRect(parent->mapToGlobal(cellrect.topLeft()), cellrect.size());
    properties["globalRect"] = PackProperty(r);

    properties["text"] = PackProperty(item_->text());
    properties["toolTip"] = PackProperty(item_->toolTip());
    properties["icon"] = item_->icon().isNull() ? PackProperty("") : PackProperty(item_->icon());
    properties["whatsThis"] = PackProperty(item_->whatsThis());
    properties["row"] = PackProperty(item_->row());
    properties["isSelected"] = PackProperty(item_->isSelected());
    properties["column"] = PackProperty(item_->column());

    return properties;
}

xpathselect::Node::Ptr QTableWidgetItemNode::GetParent() const
{
    return parent_;
}

std::string QTableWidgetItemNode::GetName() const
{
    return "QTableWidgetItem";
}

std::string QTableWidgetItemNode::GetPath() const
{
    return full_path_;
}

int32_t QTableWidgetItemNode::GetId() const
{
    // return (int32_t)((qintptr)item_);
    return static_cast<int32_t>(reinterpret_cast<qptrdiff>(item_));
}

bool QTableWidgetItemNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    Q_UNUSED(name);
    Q_UNUSED(value);
    return false;
}

bool QTableWidgetItemNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();
    return false;
}

bool QTableWidgetItemNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    Q_UNUSED(name);
    Q_UNUSED(value);
    return false;
}

xpathselect::NodeVector QTableWidgetItemNode::Children() const
{
    // Doesn't have any children.
    xpathselect::NodeVector children;
    return children;
}
