import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Controls.Material.impl
import "../"
import ScreenPlayCore as SPCore

Item {
    id: root
    objectName: "uploadProjectBigItem" + itemIndex

    property bool isProjectValid: false
    property alias checkBox: checkBox
    property bool isSelected: false
    property string customTitle: "name here"
    property string absoluteStoragePath: ""
    property string folderName: ""
    property string preview: ""
    property var type
    property bool hasMenuOpen: false
    property var publishedFileID: 0
    property int itemIndex

    signal itemClicked(var folderName, var type, var isActive)
    onTypeChanged: {
        if (type === "widget")
            icnType.source = "icons/icon_widgets.svg"
        else if (type === "qmlScene")
            icnType.source = "icons/icon_code.svg"
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
            elevation: 4
        }
    }

    Item {
        id: screenPlayItemWrapper

        anchors.fill: parent
        anchors.margins: 5

        Item {
            id: itemWrapper

            width: parent.width
            height: parent.height
            clip: true

            Image {
                id: screenPlayItemImage
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                source: root.preview !== "" ? Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.preview) : ""

                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                }
                width: parent.width * 0.55
            }

            SPCore.ColorImage {
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
                spacing: 4

                anchors {
                    top: parent.top
                    right: parent.right
                    left: screenPlayItemImage.right
                    margins: 10
                    rightMargin: 30
                }

                Text {
                    id: name

                    text: m_title
                    color: Material.foreground
                    font.pointSize: 14
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: m_typeString
                    color: Material.foreground
                    font.pointSize: 11
                    opacity: 0.7
                }
            }

            Button {
                text: qsTr("Open Folder")
                flat: true
                font.pointSize: 10
                onClicked: Qt.openUrlExternally(m_absoluteStoragePath)

                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 5
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
            objectName: "itemCheckBox" + root.itemIndex

            // Sync checkbox state with isSelected property (for delegate recycling)
            checked: root.isSelected

            onCheckedChanged: {
                if (checked !== root.isSelected) {
                    root.isSelected = checked
                    root.itemClicked(root.folderName, root.type, root.isSelected)
                }
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
