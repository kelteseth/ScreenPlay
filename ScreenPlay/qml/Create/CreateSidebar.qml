import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlay

import ScreenPlayUtil

Rectangle {
    id: root
    objectName: "createSidebar"

    property bool expanded: false
    property alias listView: listView
    property alias model: listView.model
    property StackView stackView

    width: 350
    state: expanded ? "" : "inactive"
    layer.enabled: true
    Component.onCompleted: expanded = true
    color: Material.background

    ListView {
        id: listView
        objectName: "wizardsListView"

        anchors.fill: parent
        anchors.margins: 20
        spacing: 5
        section.property: "category"

        Connections {
            id: loaderConnections

            function onWizardStarted() {
                root.expanded = false;
            }

            function onWizardExited() {
                root.expanded = true;
                App.util.setNavigation("Installed");
                App.util.setNavigationActive(true);
            }

            ignoreUnknownSignals: true
        }

        model: ListModel {
            ListElement {
                headline: qsTr("Tools Overview")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/StartInfo.qml"
                category: "Home"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Video Import h264 (.mp4)")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/Importh264/Importh264.qml"
                category: "Video Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Video Import VP8 & VP9 (.webm)")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/ImportWebm/ImportWebm.qml"
                category: "Video Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Video import (all types)")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Wallpaper"
                objectName: "videoImportConvert"
            }

            ListElement {
                headline: qsTr("GIF Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/GifWallpaper.qml"
                category: "Video Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Godot Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/GodotWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("QML Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/QMLWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("HTML5 Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/HTMLWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Website Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/WebsiteWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("QML Widget")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/QMLWidget.qml"
                category: "Code Widgets"
                objectName: ""
            }

            ListElement {
                headline: qsTr("HTML Widget")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/HTMLWidget.qml"
                category: "Code Widgets"
                objectName: ""
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AsNeeded
        }

        section.delegate: Item {
            height: 60

            Text {
                font.pointSize: 18
                color: Material.primaryTextColor
                text: section

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    bottomMargin: 10
                }
            }
        }

        delegate: Button {
            id: listItem
            objectName: model.objectName
            width: listView.width - 40
            height: 45
            highlighted: ListView.isCurrentItem
            text: headline
            onClicked: {
                listView.currentIndex = index;
                const item = stackView.push(source);
                loaderConnections.target = item;
            }
        }
    }

    layer.effect: ElevationEffect {
        elevation: 6
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
