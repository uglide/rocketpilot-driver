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

#include <QMainWindow>

class tst_Introspection : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_introspect_data();
    void test_introspect();

    void test_application_names_data();
    void test_application_names();

    void test_properties_data();
    void test_properties();

    void test_property_matching();

private:
    QMainWindow *m_object;
};
