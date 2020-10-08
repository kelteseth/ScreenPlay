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
    property bool expanded: true
    color: Material.theme === Material.Dark ? Qt.darker(
                                                  Material.background) : Material.background

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        model: ListModel {

            ListElement {
                headline: "Video import & convert (all types)"
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Import"
            }

            ListElement {
                headline: "Video Import native video (.webm)"
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Import"
            }

            ListElement {
                headline: "GIF Import"
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Import"
            }

            ListElement {
                headline: "Html Wallpaper"
                source: "qrc:/qml/Create/Wizards/CreateHTMLWallpaper.qml"
                category: "Create Wallpaper"
            }

            ListElement {
                headline: "Website Wallpaper"
                source: "qrc:/qml/Create/Wizards/CreateHTMLWallpaper.qml"
                category: "Create Wallpaper"
            }

            ListElement {
                headline: "QML Wallpaper"
                source: "qrc:/qml/Create/Wizards/CreateQMLWallpaper.qml"
                category: "Create Wallpaper"
            }

            ListElement {
                headline: "QML Widget"
                source: "qrc:/qml/Create/Wizards/CreateQMLWidget.qml"
                category: "Create Widget"
            }

            ListElement {
                headline: "HTML Widget"
                source: "qrc:/qml/Create/Wizards/CreateHTMLWidget.qml"
                category: "Create Widget"
            }

            ListElement {
                headline: "QML Particle Wallpaper"
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: "QML Water Shader Wallpaper"
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: "QML Shadertoy Shader Wallpaper"
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: "QML Lightning Shader Wallpaper"
                source: ""
                category: "Example Wallpaper"
            }

            ListElement {
                headline: "Clock Widget"
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: "CPU Widget"
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: "Storage Widget"
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: "RAM Widget"
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }

            ListElement {
                headline: "XKCD Widget"
                source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                category: "Example Widget"
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
        // snapMode: ListView.SnapToItem
        section.property: "category"
        section.delegate: Item {
            height: 60
            Text {
                verticalAlignment: Qt.AlignVCenter
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 20
                }
                font.pointSize: 18

                color: Material.primaryTextColor
                text: section
            }
        }

        delegate: ItemDelegate {
            id: listItem
            width: listView.width - 40
            height: 45
            highlighted: ListView.isCurrentItem
            onClicked: {
                listView.currentIndex = index
                loader.source = source
            }
            background: Rectangle {
                radius: 3
                layer.enabled: true
                layer.effect: ElevationEffect {
                    elevation: listItem.highlighted ? 6 : 1
                }
                color: {
                    if (Material.theme === Material.Light) {
                        return listItem.highlighted ? Material.accentColor : "white"
                    } else {
                        return listItem.highlighted ? Material.accentColor : Material.background
                    }
                }
                Behavior on color {
                    PropertyAnimation {
                        property: "color"
                        duration: 200
                        easing.type: Easing.InOutQuart
                    }
                }
            }

            Text {
                verticalAlignment: Qt.AlignVCenter
                color: Material.primaryTextColor
                text: headline
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 20
                }
                font.pointSize: 11
            }
        }
    }

    states: [
        State {
            name: ""
            PropertyChanges {
                target: root
                anchors.leftMargin:0
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: root
                anchors.leftMargin: -root.width
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "inactive"
            reversible: true
            PropertyAnimation {
                property: "anchors.leftMargin"
                duration: 250
            }
        }
    ]
}
