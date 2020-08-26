import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import QtQuick.Controls.Material.impl 2.12
import ScreenPlay 1.0

import "../"

Item {
    id: root

    height: 250
    property bool isProjectValid: false

    property alias checkBox: checkBox
    property bool isSelected: false
    property string customTitle: "name here"
    property string absoluteStoragePath: ""
    property string screenId: ""
    property string preview: ""
    property string type: ""
    property bool hasMenuOpen: false
    property int workshopID: 0
    property int itemIndex
    signal itemClicked(var screenId, var type, var isActive)

    onTypeChanged: {
        if (type === "widget") {
            icnType.source = "icons/icon_widgets.svg"
        } else if (type === "qmlScene") {
            icnType.source = "icons/icon_code.svg"
        }
    }

    //    Component.onCompleted: {
    //        print("root.preview",root.preview)
    //        if (root.preview == undefined) {
    //            print("invalid")
    //        } else {
    //            root.isProjectValid = true
    //        }

    //        if (!isProjectValid) {
    //            root.state = "invalid"
    //        }
    //    }
    Rectangle {
        anchors.fill: screenPlayItemWrapper
        radius: 4
        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 6
        }
        color: Material.theme === Material.Light ? "white" : Material.background
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
                width: 400
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                }
                source: Qt.resolvedUrl(
                            root.absoluteStoragePath + "/" + root.preview)
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
                anchors {
                    top: parent.top
                    right: parent.right
                    left: screenPlayItemImage.right
                    margins: 20
                }
                spacing: 10

                Text {
                    id: name
                    text: screenTitle
                    color: Material.foreground
                    font.pointSize: 18
                    font.family: ScreenPlay.settings.font
                    font.weight: Font.Thin
                }

                Text {
                    text: qsTr("Type: ") + screenType
                    color: Material.foreground
                    font.family: ScreenPlay.settings.font
                }
            }

            Button {
                text: qsTr("Open Folder")
                onClicked: ScreenPlay.util.openFolderInExplorer(
                               screenAbsoluteStoragePath)
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }
            }

            Rectangle {
                id: rctInvalid
                anchors.fill: parent
                color: screenPlayItemWrapper.color
                opacity: 0
                visible: false
            }

            Text {
                id: txtInvalidError
                text: qsTr("Invalid Project!")
                color: Material.color(Material.Red)
                anchors.fill: screenPlayItemImage
                font.pointSize: 18
                font.family: ScreenPlay.settings.font
                font.weight: Font.Thin
                opacity: 0
            }
        }

        CheckBox {
            id: checkBox
            onCheckStateChanged: {
                if (checkState == Qt.Checked) {
                    isSelected = true
                } else {
                    isSelected = false
                }
                itemClicked(screenId, type, isSelected)
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
                opacity: .5
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

            PropertyChanges {
                target: rctInvalid
                opacity: .8
                visible: true
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

/*##^##
Designer {
    D{i:0;formeditorZoom:0.6600000262260437;height:250;width:600}
}
##^##*/

