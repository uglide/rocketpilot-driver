/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#include <QTest>

#include "tst_qtnode.h"
#include "tst_introspection.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    tst_Introspection introspection_tc;
    tst_qtnode qtnode_tc;
    return QTest::qExec(&introspection_tc, argc, argv) || QTest::qExec(&qtnode_tc, argc, argv);
}
