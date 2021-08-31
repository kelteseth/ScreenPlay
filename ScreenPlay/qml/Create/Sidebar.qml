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


        /*
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
            */
        id: listView
        objectName: "wizardsListView"

        anchors.fill: parent
        anchors.margins: 20
        spacing: 5
        section.property: "category"

        Connections {
            id: loaderConnections

            function onWizardStarted() {
                root.expanded = false
            }

            function onWizardExited() {
                root.expanded = true
                stackView.clear(StackView.PushTransition)
                stackView.push("qrc:/qml/Create/StartInfo.qml")
                listView.currentIndex = 0
                ScreenPlay.util.setNavigationActive(true)
            }

            ignoreUnknownSignals: true
        }

        model: ListModel {
            ListElement {
                headline: qsTr("Tools Overview")
                source: "qrc:/qml/Create/StartInfo.qml"
                category: "Home"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Video import and convert (all types)")
                source: "qrc:/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Wallpaper"
                objectName: "videoImportConvert"
            }

            ListElement {
                headline: qsTr("Video Import (.webm)")
                source: "qrc:/qml/Create/Wizards/ImportWebm/ImportWebm.qml"
                category: "Video Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("GIF Wallpaper")
                source: "qrc:/qml/Create/Wizards/GifWallpaper.qml"
                category: "Video Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("QML Wallpaper")
                source: "qrc:/qml/Create/Wizards/QMLWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("HTML5 Wallpaper")
                source: "qrc:/qml/Create/Wizards/HTMLWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("Website Wallpaper")
                source: "qrc:/qml/Create/Wizards/WebsiteWallpaper.qml"
                category: "Code Wallpaper"
                objectName: ""
            }

            ListElement {
                headline: qsTr("QML Widget")
                source: "qrc:/qml/Create/Wizards/QMLWidget.qml"
                category: "Code Widgets"
                objectName: ""
            }

            ListElement {
                headline: qsTr("HTML Widget")
                source: "qrc:/qml/Create/Wizards/HTMLWidget.qml"
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
                listView.currentIndex = index
                const item = stackView.push(source)
                loaderConnections.target = item
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
