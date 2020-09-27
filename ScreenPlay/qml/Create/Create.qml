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

import "Wizards/CreateWallpaper"

Item {
    id: root
    anchors.fill: parent
    state: "out"

    Component.onCompleted: {
        root.state = "in"
    }

    BackgroundParticleSystem {
        id: particleSystemWrapper
        anchors.fill: parent
    }

    Wizard {
        id: wizard
        anchors.fill: parent
    }

    RowLayout {
        id: wrapper
        spacing: 0
        height: parent.height - footer.height
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.25
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
                        source: ""
                        category: "Video Import"
                    }

                    ListElement {
                        headline: "Video Import native video (.webm)"
                        source: ""
                        category: "Video Import"
                    }

                    ListElement {
                        headline: "GIF Import"
                        source: ""
                        category: "Video Import"
                    }

                    ListElement {
                        headline: "Html Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Create Wallpaper"
                    }

                    ListElement {
                        headline: "QML Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Create Wallpaper"
                    }

                    ListElement {
                        headline: "QML Widget"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                        category: "Create Widget"
                    }

                    ListElement {
                        headline: "HTML Widget"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                        category: "Create Widget"
                    }

                    ListElement {
                        headline: "QML Particle Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Example Wallpaper"
                    }

                    ListElement {
                        headline: "QML Water Shader Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Example Wallpaper"
                    }

                    ListElement {
                        headline: "QML Shadertoy Shader Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Example Wallpaper"
                    }

                    ListElement {
                        headline: "QML Lightning Shader Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                        category: "Example Wallpaper"
                    }

                    ListElement {
                        headline: "HTML IFrame Wallpaper"
                        source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
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
        }
        Item {
            Layout.fillHeight: true

            Layout.preferredWidth: parent.width * 0.75

            Rectangle {
                radius: 4
                layer.enabled: true
                layer.effect: ElevationEffect {
                    elevation: 6
                }
                color: Material.theme === Material.Light ? "white" : Material.background
                anchors {
                    fill: parent
                    margins: 20
                }
            }

            Loader {
                id: loader
                anchors.fill: parent
            }
        }
    }

    Footer {
        id: footer
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    states: [
        State {
            name: "out"

            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }

            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 0
            }
        },
        State {
            name: "in"

            PropertyChanges {
                target: footer
                anchors.bottomMargin: 0
            }

            PropertyChanges {
                target: wrapper
                opacity: 1
                anchors.topMargin: wrapper.spacing
            }
        },
        State {
            name: "wizard"

            PropertyChanges {
                target: footer
                anchors.bottomMargin: -80
            }
            PropertyChanges {
                target: wizard
                z: 99
            }
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 100
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"
            reversible: true

            PropertyAnimation {
                targets: [footer, wrapper]
                properties: "anchors.bottomMargin, anchors.topMargin, opacity"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        },
        Transition {
            from: "in"
            to: "wizard"
            reversible: true

            PropertyAnimation {
                targets: [footer, wrapper]
                properties: "anchors.bottomMargin, anchors.topMargin, opacity"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}
}
 ##^##*/

