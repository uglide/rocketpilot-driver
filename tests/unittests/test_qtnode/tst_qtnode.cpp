/*
 * Copyright (C) 2013 Canonical, Ltd.
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

#include <QModelIndex>
#include <QStandardItemModel>

int32_t calclulate_ap_id(quint64 big_id);
void CollectAllIndices(QModelIndex index, QAbstractItemModel *model, QModelIndexList &collection);

class tst_qtnode: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanup();
    void test_calclulate_ap_id_data();
    void test_calclulate_ap_id();

    void test_CollectAllIndices_collects_all_table_data();
    void test_CollectAllIndices_collects_all_table();
    void test_CollectAllIndices_collects_all_list_data();
    void test_CollectAllIndices_collects_all_list();
    void test_CollectAllIndices_ignores_invalids_data();
    void test_CollectAllIndices_ignores_invalids();
private:
    QStandardItemModel *testModel;
};


void tst_qtnode::initTestCase()
{
    testModel = 0;
}

void tst_qtnode::cleanup()
{
    if(testModel) {
        delete testModel;
        testModel = 0;
    }
}

void tst_qtnode::test_calclulate_ap_id_data()
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

void tst_qtnode::test_calclulate_ap_id()
{
    QFETCH(quint64, id);
    QFETCH(int32_t, expected_result);

    QCOMPARE(calclulate_ap_id(id), expected_result);
}

void tst_qtnode::test_CollectAllIndices_collects_all_table_data()
{
    int row_count = 2;
    int col_count = 2;
    testModel = new QStandardItemModel(row_count, col_count);

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
    CollectAllIndices(root_item->index(), testModel, collection);

    QCOMPARE(collection.size(), 4);
}

void tst_qtnode::test_CollectAllIndices_collects_all_list_data()
{
    int listitem_count = 4;
    testModel = new QStandardItemModel();
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
    CollectAllIndices(root_item->index(), testModel, collection);

    QCOMPARE(collection.size(), 4);
}

void tst_qtnode::test_CollectAllIndices_ignores_invalids()
{
    QModelIndexList collection;
    QStandardItem *root_item = testModel->invisibleRootItem();
    CollectAllIndices(root_item->index(), testModel, collection);

    QCOMPARE(collection.size(), 4);
}

QTEST_MAIN(tst_qtnode)
#include "tst_qtnode.moc"
