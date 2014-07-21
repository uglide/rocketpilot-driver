/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *  Christopher Lee <chris.lee@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtTest>

#include <QTreeView>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListView>
#include <QModelIndex>
#include <QStandardItemModel>

#include "tst_qtnode.h"

#include "introspection.h"
#include "qtnode.h"

int32_t calculate_ap_id(quint64 big_id);
void CollectSpecialChildren(QObject* object, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void CollectAllIndices(QModelIndex index, QAbstractItemModel *model, QModelIndexList &collection);
bool MatchProperty(const QVariantMap& packed_properties, const std::string& name, QVariant value);

void GetDataElementChildren(QTableWidget* table, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetDataElementChildren(QTreeView* tree_view, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetDataElementChildren(QTreeWidget* tree_widget, xpathselect::NodeVector& children, DBusNode::Ptr parent);
void GetDataElementChildren(QListView* list_view, xpathselect::NodeVector& children, DBusNode::Ptr parent);


void tst_qtnode::initTestCase()
{
    QApplication::setApplicationName("tst_qtnode");
}

void tst_qtnode::test_calculate_ap_id_data()
{
    QTest::addColumn<quint64>("id");
    QTest::addColumn<int32_t>("expected_result");

    QTest::newRow("1") << Q_UINT64_C(0xFFFFFFFF) << int(0xFFFFFFFF);
    QTest::newRow("2") << Q_UINT64_C(0x00000000FFFFFFFF) << int(0xFFFFFFFF);
    QTest::newRow("3") << Q_UINT64_C(0xFFFFFFFFFFFFFFFF) << int(0x0);
    QTest::newRow("4") << Q_UINT64_C(0x0F0F0F0F0F0F0F0F) << int(0x0);
    QTest::newRow("5") << Q_UINT64_C(0xF0F0F0FF0F0F0F0) << int(0xFFFFFFFF);
    QTest::newRow("6") << Q_UINT64_C(0xF0F0F0F0FFFFFFFF) << int(0xF0F0F0F);
}

void tst_qtnode::test_calculate_ap_id()
{
    QFETCH(quint64, id);
    QFETCH(int32_t, expected_result);

    QCOMPARE(calculate_ap_id(id), expected_result);
}

void tst_qtnode::test_CollectAllIndices_collects_all_table_data()
{
    int row_count = 2;
    int col_count = 2;
    testModel = std::make_shared<QStandardItemModel>(row_count, col_count);

    for (int row = 0; row < row_count; ++row) {
        for (int column = 0; column < col_count; ++column) {
            QStandardItem *item = new QStandardItem(
                QString("row %0, column %1").arg(row).arg(column));
            testModel->setItem(row, column, item);
        }
    }
}

void tst_qtnode::test_CollectAllIndices_collects_all_table()
{
    QModelIndexList collection;
    QStandardItem *root_item = testModel->invisibleRootItem();
    CollectAllIndices(root_item->index(), testModel.get(), collection);

    QCOMPARE(collection.size(), 4);
}

void tst_qtnode::test_CollectAllIndices_collects_all_list_data()
{
    int listitem_count = 4;
    testModel = std::make_shared<QStandardItemModel>();
    QStandardItem *parentItem = testModel->invisibleRootItem();
    for (int i = 0; i < listitem_count; ++i) {
        QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
        parentItem->appendRow(item);
        parentItem = item;
    }
}

void tst_qtnode::test_CollectAllIndices_collects_all_list()
{
    QModelIndexList collection;
    QStandardItem *root_item = testModel->invisibleRootItem();
    CollectAllIndices(root_item->index(), testModel.get(), collection);

    QCOMPARE(collection.size(), 4);
}

Q_DECLARE_METATYPE(std::string)
void tst_qtnode::test_MatchProperty_data()
{
    QTest::addColumn<QVariantMap>("packedProperties");
    QTest::addColumn<std::string>("name");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<bool>("expectedResult");

    QVariantMap p;
    p["string"] = PackProperty(QVariant("string"));
    p["int"] = PackProperty(QVariant(1));
    p["bool"] = PackProperty(QVariant(true));

    QTest::newRow("Matches string") << p << std::string("string") << QVariant("string") << true;
    QTest::newRow("Matches int") << p << std::string("int") << QVariant(1) << true;
    QTest::newRow("Matches bool") << p << std::string("bool") << QVariant(true) << true;

    QTest::newRow("Fails not present") << p << std::string("notpresent") << QVariant("string") << false;
    QTest::newRow("Fails values do not match")
        << p
        << std::string("string")
        << QVariant("notstring")
        << false;
}

void tst_qtnode::test_MatchProperty()
{
    QFETCH(QVariantMap, packedProperties);
    QFETCH(std::string, name);
    QFETCH(QVariant, value);
    QFETCH(bool, expectedResult);

    QCOMPARE(MatchProperty(packedProperties, name, value), expectedResult);
}

void tst_qtnode::populate_QTreeView_with_data()
{
    testModel = std::make_shared<QStandardItemModel>();
    testModel->setColumnCount(1);
    testModel->setRowCount(5);
    testModel->setData(testModel->index(0, 0), "test0");
    testModel->setData(testModel->index(1, 0), "test1");
    testModel->setData(testModel->index(2, 0), "test2");
    testModel->setData(testModel->index(3, 0), "test3");
    testModel->setData(testModel->index(4, 0), "test4");

    treeView = std::make_shared<QTreeView>();
    treeView->setModel(testModel.get());
}

void tst_qtnode::populate_QTreeWidget_with_data()
{
    treeWidget = std::make_shared<QTreeWidget>();
    treeWidget->setColumnCount(1);
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 5; ++i)
        items.append(new QTreeWidgetItem());
    treeWidget->insertTopLevelItems(0, items);
}

void tst_qtnode::populate_QListView_with_data()
{
    testModel = std::make_shared<QStandardItemModel>();
    testModel->setColumnCount(1);
    testModel->setRowCount(5);
    testModel->setData(testModel->index(0, 0), "test0");
    testModel->setData(testModel->index(1, 0), "test1");
    testModel->setData(testModel->index(2, 0), "test2");
    testModel->setData(testModel->index(3, 0), "test3");
    testModel->setData(testModel->index(4, 0), "test4");

    listView = std::make_shared<QListView>();
    listView->setModel(testModel.get());
}

void tst_qtnode::populate_QTableWidget_with_data()
{
    tableWidget = std::make_shared<QTableWidget>();
    tableWidget->setRowCount(4);
    tableWidget->setColumnCount(2);

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 2; ++column) {
            tableWidget->setItem(row, column, new QTableWidgetItem());
        }
    }
}

void tst_qtnode::test_GetDataElementChildren_QTreeView_collects_all_data()
{
    populate_QTreeView_with_data();
}

void tst_qtnode::test_GetDataElementChildren_QTreeView_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    GetDataElementChildren(treeView.get(), children, parent);

    QCOMPARE((int)children.size(), 5);

    auto node_parent = children[0]->GetParent();
    QVERIFY(node_parent == parent);
}

void tst_qtnode::test_GetDataElementChildren_QTreeWidget_collects_all_data()
{
    populate_QTreeWidget_with_data();
}

void tst_qtnode::test_GetDataElementChildren_QTreeWidget_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;
    GetDataElementChildren(treeWidget.get(), children, parent);

    QCOMPARE((int)children.size(), 5);

    auto node_parent = children[0]->GetParent();
    QVERIFY(node_parent == parent);
}

void tst_qtnode::test_GetDataElementChildren_QListView_collects_all_data()
{
    populate_QListView_with_data();
}

void tst_qtnode::test_GetDataElementChildren_QListView_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    GetDataElementChildren(listView.get(), children, parent);

    QCOMPARE((int)children.size(), 5);

    auto node_parent = children[0]->GetParent();
    QVERIFY(node_parent == parent);
}

void tst_qtnode::test_GetDataElementChildren_QTableWidget_collects_all_data()
{
    populate_QTableWidget_with_data();
}

void tst_qtnode::test_GetDataElementChildren_QTableWidget_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    GetDataElementChildren(tableWidget.get(), children, parent);

    QCOMPARE((int)children.size(), 8);

    auto node_parent = children[0]->GetParent();
    QVERIFY(node_parent == parent);
}

void tst_qtnode::test_CollectSpecialChildren_QTreeView_collects_all_data()
{
    populate_QTreeView_with_data();
}

void tst_qtnode::test_CollectSpecialChildren_QTreeView_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    CollectSpecialChildren(treeView.get(), children, parent);

    QCOMPARE((int)children.size(), 5);
}

void tst_qtnode::test_CollectSpecialChildren_QTreeWidget_collects_all_data()
{
    populate_QTreeWidget_with_data();
}

void tst_qtnode::test_CollectSpecialChildren_QTreeWidget_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    CollectSpecialChildren(treeWidget.get(), children, parent);

    QCOMPARE((int)children.size(), 5);
}

void tst_qtnode::test_CollectSpecialChildren_QListView_collects_all_data()
{
    populate_QListView_with_data();
}

void tst_qtnode::test_CollectSpecialChildren_QListView_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    CollectSpecialChildren(listView.get(), children, parent);

    QCOMPARE((int)children.size(), 5);
}

void tst_qtnode::test_CollectSpecialChildren_QTableWidget_collects_all_data()
{
    populate_QTableWidget_with_data();
}

void tst_qtnode::test_CollectSpecialChildren_QTableWidget_collects_all()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;

    CollectSpecialChildren(tableWidget.get(), children, parent);

    QCOMPARE((int)children.size(), 8);
}

void tst_qtnode::test_CollectSpecialChildren_QObject_collects_nothing()
{
    xpathselect::NodeVector children;
    DBusNode::Ptr parent;
    std::shared_ptr<QObject> testObject = std::make_shared<QObject>();

    CollectSpecialChildren(testObject.get(), children, parent);

    QCOMPARE((int)children.size(), 0);
}
