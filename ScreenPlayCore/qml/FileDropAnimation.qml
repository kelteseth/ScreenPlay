import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects

Item {
    id: root
    height: 400
    width: 400

    property int secondaryIconSize: 180
    property int centerIconSize: 210
    property int growSize: 40

    Image {
        id: fileRight
        opacity: 0
        width: root.secondaryIconSize
        height: root.secondaryIconSize
        rotation: -5
        smooth: true
        antialiasing: true
        mipmap: true
        source: "qrc:/qml/ScreenPlayCore/assets/icons/attach_file.svg"
        sourceSize: Qt.size(width, width)
        layer {
            enabled: true
            smooth: true
            effect: ColorOverlay {
                color: Material.iconColor
            }
        }
        anchors {
            top: parent.top
            topMargin: 150
            left: fileCenter.horizontalCenter
            leftMargin: -root.secondaryIconSize
        }
    }
    Image {
        id: fileLeft
        opacity: 0
        width: root.secondaryIconSize
        height: root.secondaryIconSize
        source: "qrc:/qml/ScreenPlayCore/assets/icons/description.svg"
        sourceSize: Qt.size(width, width)
        layer {
            enabled: true
            smooth: true
            effect: ColorOverlay {
                color: Material.iconColor
            }
        }
        rotation: 5
        smooth: true
        antialiasing: true
        mipmap: true
        anchors {
            top: parent.top
            topMargin: 150
            right: fileCenter.horizontalCenter
            rightMargin: -root.secondaryIconSize
        }
    }
    Image {
        id: fileCenter
        opacity: 0
        width: root.centerIconSize
        height: root.centerIconSize
        source: "qrc:/qml/ScreenPlayCore/assets/icons/folder.svg"
        sourceSize: Qt.size(width, width)
        layer {
            enabled: true
            smooth: true
            effect: ColorOverlay {
                color: Material.color(Material.Orange)
            }
        }
        smooth: true
        antialiasing: true
        mipmap: true
        anchors {
            top: parent.top
            topMargin: 150
            horizontalCenter: parent.horizontalCenter
        }
        Material.elevation: 6
    }

    Text {
        id: txt
        color: Material.primaryTextColor
        opacity: 0
        anchors.top: fileCenter.bottom
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 16
        text: qsTr("Drop your '.screenplay' file here to add it to your Installed content.")
        horizontalAlignment: Text.AlignHCenter
    }

    states: [
        State {
            name: "fileDrop"
            PropertyChanges {
                target: fileCenter
                opacity: 1
                anchors.topMargin: 100
                width: root.centerIconSize + root.growSize
                height: root.centerIconSize + root.growSize
            }
            PropertyChanges {
                target: txt
                anchors.topMargin: 20
                opacity: 1
            }
            PropertyChanges {
                target: fileRight
                opacity: 1
                anchors.topMargin: 100
                anchors.leftMargin: -170 - root.growSize
                width: root.secondaryIconSize + root.growSize
                height: root.secondaryIconSize + root.growSize
                rotation: -10
            }
            PropertyChanges {
                target: fileLeft
                opacity: 1
                anchors.topMargin: 100
                anchors.rightMargin: -170 - root.growSize
                width: root.secondaryIconSize + root.growSize
                height: root.secondaryIconSize + root.growSize
                rotation: 10
            }
        }
    ]

    transitions: Transition {
        AnchorAnimation {
            duration: 400
        }
        PropertyAnimation {
            target: fileCenter
            properties: "anchors.topMargin, width, height, opacity"
            duration: 400
            easing.type: Easing.OutCirc
        }
        PropertyAnimation {
            target: txt
            properties: "anchors.topMargin, opacity"
            duration: 400
            easing.type: Easing.OutCirc
        }
        PropertyAnimation {
            target: fileRight
            properties: "anchors.topMargin,anchors.leftMargin, width, height, rotation, opacity"
            duration: 400
            easing.type: Easing.OutCirc
        }
        PropertyAnimation {
            target: fileLeft
            properties: "anchors.topMargin, anchors.rightMargin, width, height, rotation, opacity"
            duration: 400
            easing.type: Easing.OutCirc
        }

        RotationAnimation {
            target: fileLeft
            duration: 800
        }
        RotationAnimation {
            target: fileRight
            duration: 800
        }
    }
}
