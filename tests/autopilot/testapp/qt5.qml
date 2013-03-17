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
