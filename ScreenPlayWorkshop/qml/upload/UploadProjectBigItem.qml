import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import "../"

Item {
    id: root

    property bool isProjectValid: false
    property alias checkBox: checkBox
    property bool isSelected: false
    property string customTitle: "name here"
    property string absoluteStoragePath: ""
    property string screenId: ""
    property string preview: ""
    property var type
    property bool hasMenuOpen: false
    property var publishedFileID: 0
    property int itemIndex

    signal itemClicked(var screenId, var type, var isActive)

    height: 250
    onTypeChanged: {
        if (type === "widget")
            icnType.source = "icons/icon_widgets.svg";
        else if (type === "qmlScene")
            icnType.source = "icons/icon_code.svg";
    }

    //        if (!isProjectValid) {
    //            root.state = "invalid"
    //        }
    //    }
    Rectangle {
        anchors.fill: screenPlayItemWrapper
        radius: 4
        layer.enabled: true
        color: Material.theme === Material.Light ? "white" : Material.background

        layer.effect: ElevationEffect {
            elevation: 6
        }
    }

    Item {
        id: screenPlayItemWrapper

        anchors.fill: parent
        anchors.margins: 20

        Item {
            id: itemWrapper

            width: parent.width
            height: parent.height
            clip: true

            Image {
                id: screenPlayItemImage
                asynchronous: true
                width: 400
                source: root.preview !== "" ? Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.preview) : ""

                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                }
            }

            Image {
                id: icnType

                width: 20
                height: 20
                sourceSize: Qt.size(20, 20)

                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
            }

            ColumnLayout {
                spacing: 10

                anchors {
                    top: parent.top
                    right: parent.right
                    left: screenPlayItemImage.right
                    margins: 20
                }

                Text {
                    id: name

                    text: m_title
                    color: Material.foreground
                    font.pointSize: 18
                }

                Text {
                    text: qsTr("Type: ") + m_type
                    color: Material.foreground
                }
            }

            Button {
                text: qsTr("Open Folder")
                onClicked: App.util.openFolderInExplorer(m_absoluteStoragePath)

                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }
            }

            Text {
                id: txtInvalidError

                text: qsTr("Invalid Project!")
                color: Material.color(Material.Red)
                anchors.fill: screenPlayItemImage
                font.pointSize: 18
                opacity: 0
            }
        }

        CheckBox {
            id: checkBox

            onCheckStateChanged: {
                if (checkState == Qt.Checked)
                    isSelected = true;
                else
                    isSelected = false;
                itemClicked(screenId, type, isSelected);
            }

            anchors {
                top: parent.top
                right: parent.right
                margins: 10
            }
        }
    }

    states: [
        State {
            name: "selected"

            PropertyChanges {
                target: screenPlayItemWrapper
                y: 0
                opacity: 1
            }

            PropertyChanges {
                target: icnType
                opacity: 0.5
            }
        },
        State {
            name: "invalid"

            PropertyChanges {
                target: checkBox
                enabled: false
            }

            PropertyChanges {
                target: txtInvalidError
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "invalid"

            PropertyAnimation {
                property: opacity
                target: txtInvalidError
                duration: 250
            }
        }
    ]
}
