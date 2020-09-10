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
        checkFFMPEG()
    }

    function checkFFMPEG() {
        if (!ScreenPlay.util.ffmpegAvailable) {
            ffmpegPopup.open()
        }
    }

    FFMPEGPopup {
        id: ffmpegPopup
        anchors.centerIn: root
        closePolicy: Popup.NoAutoClose
        focus: true
        modal: true
        parent: root
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
        height: parent.height - (footer.height + 80)
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 40
        }
        spacing: 40

        ListView {
            id: listView
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.25
            currentIndex: 0
            spacing: 10
            model: ListModel {

                ListElement {
                    headline: "Empty Html Wallpaper"
                    source: "qrc:/qml/Create/Wizards/CreateEmptyHtmlWallpaper/CreateEmptyHtmlWallpaper.qml"
                    category: "create"
                }

                ListElement {
                    headline: "Empty Widget"
                    source: "qrc:/qml/Create/Wizards/CreateEmptyWidget/CreateEmptyWidget.qml"
                    category: "create"
                }

                ListElement {
                    headline: "Create3"
                    source: ""
                    category: "create"
                }

                ListElement {
                    headline: "1"
                    source: ""
                    category: "import"
                }

                ListElement {
                    headline: "2"
                    source: ""
                    category: "import"
                }

                ListElement {
                    headline: "Create3"
                    source: ""
                    category: "create"
                }

                ListElement {
                    headline: "1"
                    source: ""
                    category: "import"
                }

                ListElement {
                    headline: "2"
                    source: ""
                    category: "import"
                }
                ListElement {
                    headline: "Create3"
                    source: ""
                    category: "create"
                }

                ListElement {
                    headline: "1"
                    source: ""
                    category: "import"
                }

                ListElement {
                    headline: "2"
                    source: ""
                    category: "import"
                }
            }

            ScrollBar.vertical: ScrollBar {
                snapMode: ScrollBar.SnapOnRelease
                policy: ScrollBar.AlwaysOn
            }
            snapMode: ListView.SnapToItem
            section.property: "category"
            section.delegate: Item {
                height: 80
                Text {
                    verticalAlignment: Qt.AlignVCenter
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                    font.pointSize: 16

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
                    print(source)
                    loader.source = source
                }
               // required property string source
                background: Rectangle {
                    radius: 4
                    layer.enabled: true
                    layer.effect: ElevationEffect {
                        elevation: 6
                    }
                    color: {
                        if (Material.theme === Material.Light) {
                            return listItem.highlighted ? Material.accentColor : "white"
                        } else {
                            return listItem.highlighted ? Material.accentColor : Material.background
                        }
                    }
                }

                Text {
                    verticalAlignment: Qt.AlignVCenter
                    color: Material.secondaryTextColor
                    text: headline
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 20
                    }
                    font.pointSize: 14
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.75 - parent.spacing
            radius: 4
            layer.enabled: true
            layer.effect: ElevationEffect {
                elevation: 6
            }
            color: Material.theme === Material.Light ? "white" : Material.background

            Loader {
                id: loader
                anchors {
                    fill: parent
                    margins: 20
                }
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

