import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material.impl 2.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import ScreenPlay.QMLUtilities 1.0

Rectangle {
    id: root
    width: 350
    state: expanded ? "" : "inactive"
    layer.enabled: true
    layer.effect: ElevationEffect {
        elevation: 6
    }
    property bool expanded: false
    Component.onCompleted: expanded = true
    color: Material.background

    property alias listView: listView
    property alias model: listView.model
    property StackView stackView

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 20
        spacing: 5
        model: ListModel {

            ListElement {
                headline: qsTr("Tools Overview")
                source: "qrc:/qml/Create/StartInfo.qml"
                category: "Home"
            }

            ListElement {
                headline: "Video import and convert (all types)"
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Import Video"
            }

            ListElement {
                headline: qsTr("Video Import (.webm)")
                source: "qrc:/qml/Create/Wizards/ImportWebm/ImportWebm.qml"
                category: "Import Video"
            }

            ListElement {
                headline: qsTr("GIF Import")
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Import Video"
            }

            ListElement {
                headline: qsTr("QML Wallpaper")
                source: "qrc:/qml/Create/Wizards/CreateQMLWallpaper.qml"
                category: "Create Wallpaper"
            }

            ListElement {
                headline: qsTr("HTML5 Wallpaper")
                source: "qrc:/qml/Create/Wizards/CreateHTMLWallpaper.qml"
                category: "Create Wallpaper"
            }

            ListElement {
                headline: qsTr("Embedded Website Wallpaper")
                source: "qrc:/qml/Create/Wizards/CreateHTMLWallpaper.qml"
                category: "Create Wallpaper"
            }
            ListElement {
                headline: qsTr("QML Widget")
                source: "qrc:/qml/Create/Wizards/CreateQMLWidget.qml"
                category: "Create Widget"
            }

            ListElement {
                headline: qsTr("HTML Widget")
                source: "qrc:/qml/Create/Wizards/CreateHTMLWidget.qml"
                category: "Create Widget"
            }

            ListElement {
                headline: qsTr("QML Particle Wallpaper")
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: qsTr("Countdown Clock Wallpaper")
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: qsTr("QML Water Shader Wallpaper")
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: qsTr("QML Shadertoy Shader Wallpaper")
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: qsTr("QML Lightning Shader Wallpaper")
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: qsTr("Clock Widget")
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: qsTr("CPU Widget")
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: qsTr("Storage Widget")
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: qsTr("RAM Widget")
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: qsTr("XKCD Widget")
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
        section.property: "category"
        section.delegate: Item {
            height: 60
            Text {
                //verticalAlignment: Qt.AlignVCenter
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    bottomMargin: 10
                }
                font.pointSize: 18

                color: Material.primaryTextColor
                text: section
            }
        }

        Connections {
            id: loaderConnections
            ignoreUnknownSignals: true
            function onWizardStarted() {
                root.expanded = false
            }
            function onWizardExited() {
                root.expanded = true

                stackView.clear(StackView.PushTransition)
                stackView.push("qrc:/qml/Create/StartInfo.qml")
            }
        }

        delegate: Button {
            id: listItem
            width: listView.width - 40
            height: 45
            highlighted: ListView.isCurrentItem
            onClicked: {
                listView.currentIndex = index
                const item = stackView.push(source)
                loaderConnections.target = item
            }
            text: headline
        }
    }

    states: [
        State {
            name: ""
            PropertyChanges {
                target: root
                anchors.leftMargin: 0
                opacity: 1
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: root
                opacity: 0
                anchors.leftMargin: -root.width
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "inactive"
            reversible: true
            SequentialAnimation {

                PauseAnimation {
                    duration: 100
                }
                PropertyAnimation {
                    properties: "anchors.leftMargin,opacity"
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }
        }
    ]
}
