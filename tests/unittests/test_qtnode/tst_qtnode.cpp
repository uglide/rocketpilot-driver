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

int32_t calclulate_ap_id(quint64 big_id);

class tst_qtnode: public QObject
{
    Q_OBJECT

private slots:
    void test_calclulate_ap_id_data();
    void test_calclulate_ap_id();
};

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

QTEST_MAIN(tst_qtnode)
#include "tst_qtnode.moc"
