/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Michael Zanetti <michael.zanetti@canonical.com>
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

import QtQuick 2.0

Item {
    id: root
    objectName: "rootItem"
    width: 500
    height: 500

    Item {
        id: testItem
        objectName: "testItem"

        property string stringProperty: "Testing rocks, debugging sucks!"
        property int intProperty: 42
        property bool boolProperty: false
        property real realProperty: 0.42
        property double doubleProperty: 0.42
    }

    Rectangle {
        id: rect
        objectName: "testRectangle"
        anchors.fill: parent
        color: "blue"
    }

    MouseArea {
        objectName: "testMouseArea"
        width: root.width / 2
        height: root.height / 2
        anchors.centerIn: root

        onClicked: rect.color = "red"
    }
}
