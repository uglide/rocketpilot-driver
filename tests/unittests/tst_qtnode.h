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

#include <memory>

#include <QObject>

class QStandardItemModel;
class QStandardItemModel;
class QTreeWidget;
class QListView;
class QTreeView;
class QTableWidget;

class tst_qtnode: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void test_calculate_ap_id_data();
    void test_calculate_ap_id();

    void test_CollectAllIndices_collects_all_table_data();
    void test_CollectAllIndices_collects_all_table();
    void test_CollectAllIndices_collects_all_list_data();
    void test_CollectAllIndices_collects_all_list();

    void test_MatchProperty_data();
    void test_MatchProperty();

    void populate_QTreeView_with_data();
    void populate_QTreeWidget_with_data();
    void populate_QListView_with_data();
    void populate_QTableWidget_with_data();

    void test_GetDataElementChildren_QTreeView_collects_all_data();
    void test_GetDataElementChildren_QTreeView_collects_all();
    void test_GetDataElementChildren_QTreeWidget_collects_all_data();
    void test_GetDataElementChildren_QTreeWidget_collects_all();
    void test_GetDataElementChildren_QListView_collects_all_data();
    void test_GetDataElementChildren_QListView_collects_all();
    void test_GetDataElementChildren_QTableWidget_collects_all_data();
    void test_GetDataElementChildren_QTableWidget_collects_all();

    void test_CollectSpecialChildren_QTreeView_collects_all_data();
    void test_CollectSpecialChildren_QTreeView_collects_all();
    void test_CollectSpecialChildren_QTreeWidget_collects_all_data();
    void test_CollectSpecialChildren_QTreeWidget_collects_all();
    void test_CollectSpecialChildren_QListView_collects_all_data();
    void test_CollectSpecialChildren_QListView_collects_all();

    void test_CollectSpecialChildren_QTableWidget_collects_all_data();
    void test_CollectSpecialChildren_QTableWidget_collects_all();
private:
    std::shared_ptr<QStandardItemModel> testModel;
    std::shared_ptr<QTreeWidget> treeWidget;
    std::shared_ptr<QListView> listView;
    std::shared_ptr<QTreeView> treeView;
    std::shared_ptr<QTableWidget> tableWidget;
};
