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

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QTableWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QListView>

const QByteArray AP_ID_NAME("_autopilot_id");

void GetTableWidgetChildren(QTableWidget* table, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetTreeViewChildren(QTreeView* tree_view, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetTreeWidgetChildren(QTreeWidget* tree_widget, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetListViewChildren(QListView* list_view, xpathselect::NodeVector& children, DBusNode::Ptr parent);

void CollectAllIndices(QModelIndex index, QAbstractItemModel *model, QModelIndexList &collection);
QVariant SafePackProperty(QVariant const& prop);

bool MatchProperty(const QVariantMap& packed_properties, const QString& name, QVariant& value);

inline int32_t xor_id(quint64 big_id)
{
    int32_t high = static_cast<int32_t>(big_id >> 32);
    int32_t low = static_cast<int32_t>(big_id);
    return high ^ low;
}

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

void GetTableWidgetChildren(QTableWidget *table, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    QList<QTableWidgetItem *> tablewidgetitems = table->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive);
    foreach (QTableWidgetItem *item, tablewidgetitems){
        children.push_back(
            std::make_shared<QTableWidgetItemNode>(item, parent)
            );
    }
}

void CollectAllIndices(QModelIndex index, QAbstractItemModel *model, QModelIndexList &collection)
{
    for(int c=0; c < model->columnCount(index); ++c) {
        for(int r=0; r < model->rowCount(index); ++r) {
            QModelIndex new_index = model->index(r, c, index);
            collection.push_back(new_index);
            if(new_index.isValid() && qHash(new_index) != qHash(index)) {
                CollectAllIndices(new_index, model, collection);
            }
        }
    }
}

// Pack property, but return a default blank if the packed property is invalid.
QVariant SafePackProperty(QVariant const& prop)
{
    static QVariant blank_default = PackProperty("");

    QVariant property_attempt = PackProperty(prop);
    if(property_attempt.isValid())
        return property_attempt;
    else
        return blank_default;
}

bool MatchProperty(const QVariantMap& packed_properties, const QString& name, QVariant& value)
{
    if (! packed_properties.contains(name))
        return false;

    // Because the properties are packed, we need the value, not the type.
    QVariant object_value = qvariant_cast<QVariantList>(packed_properties[name]).at(1);
    if (value.canConvert(object_value.type()))
    {
        value.convert(object_value.type());
        return value == object_value;
    }
    return false;
}


void GetTreeViewChildren(QTreeView* tree_view, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    QAbstractItemModel* abstract_model = tree_view->model();
    if(! abstract_model)
    {
        qDebug() << "Unable to retrieve model for QTreeView";
        return;
    }

    QModelIndexList all_indices;
    for(int c=0; c < abstract_model->columnCount(); ++c) {
        for(int r=0; r < abstract_model->rowCount(); ++r) {
            QModelIndex index = abstract_model->index(r, c);
            all_indices.push_back(index);
            CollectAllIndices(index, abstract_model, all_indices);
        }
    }

    foreach(QModelIndex index, all_indices)
    {
        if(index.isValid())
        {
            children.push_back(
                std::make_shared<QModelIndexNode>(
                    index,
                    tree_view,
                    parent)
                );
        }
    }
}

void GetTreeWidgetChildren(QTreeWidget* tree_widget, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    for(int i=0; i < tree_widget->topLevelItemCount(); ++i) {
        children.push_back(
            std::make_shared<QTreeWidgetItemNode>(
                tree_widget->topLevelItem(i),
                parent)
            );
    }
}

void GetListViewChildren(QListView* list_view, xpathselect::NodeVector& children, DBusNode::Ptr parent)
{
    QAbstractItemModel* abstract_model = list_view->model();

    if(! abstract_model) {
        qDebug() << "Unable to retrieve model for QTreeView";
        return;
    }

    QModelIndexList all_indices;
    QModelIndex root_index = list_view->rootIndex();
    if(root_index.isValid()) {
        // The root item is the parent item to the view's toplevel items
        CollectAllIndices(root_index, abstract_model, all_indices);
    }
    else {
        for(int c=0; c < abstract_model->columnCount(); ++c) {
            for(int r=0; r < abstract_model->rowCount(); ++r) {
                QModelIndex index = abstract_model->index(r, c);
                all_indices.push_back(index);
                CollectAllIndices(index, abstract_model, all_indices);
            }
        }
    }

    foreach(QModelIndex index, all_indices) {
        if(index.isValid())
        {
            children.push_back(
                std::make_shared<QModelIndexNode>(
                    index,
                    list_view,
                    parent)
                );
        }
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
    QVariant qvalue = QVariant(QString::fromStdString(value));
    return MatchProperty(properties, qname, qvalue);
}

bool QObjectNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();

    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

bool QObjectNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetNodeProperties(object_);

    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

xpathselect::NodeVector QObjectNode::Children() const
{
    xpathselect::NodeVector children;

    // Do any special children handling if needed.
    // Because QTreeWidget inherits from QTreeView check for it first.
    if(object_->inherits("QTableWidget"))
    {
        QTableWidget* table = qobject_cast<QTableWidget *>(object_);
        if(table) {
            GetTableWidgetChildren(table, children, shared_from_this());
        }
        else {
            qDebug() << "Casting object to QTableWidget failed. Unable to retrieve children.";
        }
    }
    else if(object_->inherits("QTreeWidget"))
    {
        QTreeWidget* tree_widget = qobject_cast<QTreeWidget *>(object_);
        if(tree_widget) {
            GetTreeWidgetChildren(tree_widget, children, shared_from_this());
        }
        else {
            qDebug() << "Casting object to QTreeWidget failed. Unable to retrieve children.";
        }
    }
    else if(object_->inherits("QTreeView"))
    {
        QTreeView* tree_view = qobject_cast<QTreeView *>(object_);
        if(tree_view) {
            GetTreeViewChildren(tree_view, children, shared_from_this());
        }
        else {
            qDebug() << "Casting object to QTreeView failed. Unable to retrieve children.";
        }
    }
    else if(object_->inherits("QListView"))
    {
        QListView* list_view = qobject_cast<QListView *>(object_);
        if(list_view) {
            GetListViewChildren(list_view, children, shared_from_this());
        }
        else {
            qDebug() << "Casting object to QTreeView failed. Unable to retrieve children.";
        }
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

// QModelIndexNode
QModelIndexNode::QModelIndexNode(QModelIndex index, QAbstractItemView* parent_view, DBusNode::Ptr parent)
    : index_(index)
    , parent_view_(parent_view)
    , parent_(parent)
{
    std::string parent_path = parent ? parent->GetPath() : "";
    full_path_ = parent_path + "/" + GetName();
}

QModelIndexNode::QModelIndexNode(QModelIndex index, QAbstractItemView* parent_view)
    : index_(index)
    , parent_view_(parent_view)
{
    full_path_ = "/" + GetName();
}

NodeIntrospectionData QModelIndexNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetProperties();
    data.state["id"] = PackProperty(GetId());
    return data;
}

QVariantMap QModelIndexNode::GetProperties() const
{
    QVariantMap properties;
    const QAbstractItemModel* model = index_.model();
    if(model)
    {
        // Make an attempt to store the 'text' of a node to be user friendly-ish.
        QVariant text_property = PackProperty(model->data(index_));
        if(text_property.isValid())
            properties["text"] = text_property;
        else
            properties["text"] = PackProperty("");

        // Include any Role data (mung the role name with added "Role")
        const QHash<int, QByteArray> role_names = model->roleNames();
        QMap<int, QVariant> item_data = model->itemData(index_);
        foreach(int name, role_names.keys())
        {
            if(item_data.contains(name)) {
                QVariant property = PackProperty(item_data[name]);
                if(property.isValid())
                    properties[role_names[name]+"Role"] = property;
            }
            else {
                properties[role_names[name]+"Role"] = PackProperty("");
            }
        }
    }

    QRect rect = parent_view_->visualRect(index_);
    QRect global_rect(
        parent_view_->viewport()->mapToGlobal(rect.topLeft()),
        rect.size());
    QRect viewport_contents = parent_view_->viewport()->contentsRect();
    properties["onScreen"] = PackProperty(viewport_contents.contains(rect));
    properties["globalRect"] = PackProperty(global_rect);

    return properties;
}

xpathselect::Node::Ptr QModelIndexNode::GetParent() const
{
    return parent_;
}

std::string QModelIndexNode::GetName() const
{
    return "QModelIndex";
}

std::string QModelIndexNode::GetPath() const
{
    return full_path_;
}

int32_t QModelIndexNode::GetId() const
{
    return xor_id(static_cast<quint64>(qHash(index_)));
}

bool QModelIndexNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(QString::fromStdString(value));
    return MatchProperty(properties, qname, qvalue);
}

bool QModelIndexNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();

    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

bool QModelIndexNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

xpathselect::NodeVector QModelIndexNode::Children() const
{
    // Doesn't have any children.
    xpathselect::NodeVector children;
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

    properties["text"] = SafePackProperty(PackProperty(item_->text()));
    properties["toolTip"] = SafePackProperty(PackProperty(item_->toolTip()));
    properties["icon"] = SafePackProperty(PackProperty(item_->icon()));
    properties["whatsThis"] = SafePackProperty(PackProperty(item_->whatsThis()));
    properties["row"] = SafePackProperty(PackProperty(item_->row()));
    properties["isSelected"] = SafePackProperty(PackProperty(item_->isSelected()));
    properties["column"] = SafePackProperty(PackProperty(item_->column()));

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
    return xor_id(static_cast<quint64>(reinterpret_cast<quintptr>(item_)));
}

bool QTableWidgetItemNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(QString::fromStdString(value));
    return MatchProperty(properties, qname, qvalue);
}

bool QTableWidgetItemNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();

    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

bool QTableWidgetItemNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

xpathselect::NodeVector QTableWidgetItemNode::Children() const
{
    // Doesn't have any children.
    xpathselect::NodeVector children;
    return children;
}

// QTreeWidgetItemNode
QTreeWidgetItemNode::QTreeWidgetItemNode(QTreeWidgetItem *item, DBusNode::Ptr parent)
    : item_(item)
    , parent_(parent)
{
    std::string parent_path = parent ? parent->GetPath() : "";
    full_path_ = parent_path + "/" + GetName();
}

QTreeWidgetItemNode::QTreeWidgetItemNode(QTreeWidgetItem *item)
    : item_(item)
{
    full_path_ = "/" + GetName();
}

NodeIntrospectionData QTreeWidgetItemNode::GetIntrospectionData() const
{
    NodeIntrospectionData data;
    data.object_path = QString::fromStdString(GetPath());
    data.state = GetProperties();
    data.state["id"] = PackProperty(GetId());
    return data;
}

QVariantMap QTreeWidgetItemNode::GetProperties() const
{
    QVariantMap properties;
    QTreeWidget* parent = item_->treeWidget();
    QRect cellrect = parent->visualItemRect(item_);
    QRect r = QRect(parent->viewport()->mapToGlobal(cellrect.topLeft()), cellrect.size());
    properties["globalRect"] = PackProperty(r);

    properties["text"] = SafePackProperty(item_->text(0));
    properties["columns"] = SafePackProperty(item_->columnCount());
    properties["checkState"] = SafePackProperty(item_->checkState(0));

    properties["isDisabled"] = SafePackProperty(item_->isDisabled());
    properties["isExpanded"] = SafePackProperty(item_->isExpanded());
    properties["isFirstColumnSpanned"] = SafePackProperty(item_->isFirstColumnSpanned());
    properties["isHidden"] = SafePackProperty(item_->isHidden());
    properties["isSelected"] = SafePackProperty(item_->isSelected());

    return properties;
}

xpathselect::Node::Ptr QTreeWidgetItemNode::GetParent() const
{
    return parent_;
}

std::string QTreeWidgetItemNode::GetName() const
{
    return "QTreeWidgetItem";
}

std::string QTreeWidgetItemNode::GetPath() const
{
    return full_path_;
}

int32_t QTreeWidgetItemNode::GetId() const
{
    return xor_id(static_cast<quint64>(reinterpret_cast<quintptr>(item_)));
}

bool QTreeWidgetItemNode::MatchStringProperty(const std::string& name, const std::string& value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(QString::fromStdString(value));
    return MatchProperty(properties, qname, qvalue);
}

bool QTreeWidgetItemNode::MatchIntegerProperty(const std::string& name, int32_t value) const
{
    if (name == "id")
        return value == GetId();

    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

bool QTreeWidgetItemNode::MatchBooleanProperty(const std::string& name, bool value) const
{
    QVariantMap properties = GetProperties();
    QString qname = QString::fromStdString(name);
    QVariant qvalue = QVariant(value);
    return MatchProperty(properties, qname, qvalue);
}

xpathselect::NodeVector QTreeWidgetItemNode::Children() const
{
    xpathselect::NodeVector children;

    for(int i=0; i < item_->childCount(); ++i) {
        children.push_back(
            std::make_shared<QTreeWidgetItemNode>(item_->child(i),shared_from_this())
            );
    }

    return children;
}
