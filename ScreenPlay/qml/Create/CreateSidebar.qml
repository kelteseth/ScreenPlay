import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material.impl
import ScreenPlayApp
import ScreenPlayUtil
import "../Components"

Rectangle {
    id: root
    objectName: "createSidebar"

    property bool expanded: false
    property alias listView: listView
    property alias model: listView.model
    property StackView stackView

    width: 340
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
                root.expanded = false
            }

            function onWizardExited() {
                root.expanded = true
                App.util.setNavigation("Installed")
                App.util.setNavigationActive(true)
            }

            ignoreUnknownSignals: true
        }

        model: ListModel {
            ListElement {
                headline: qsTr("Tools Overview")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/StartInfo.qml"
                category: "Home"
                proFeature: false
            }

            ListElement {
                headline: qsTr("Import Video")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Wallpaper"
                objectName: "videoImportConvert"
                proFeature: false
            }

            ListElement {
                headline: qsTr("GIF Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/GifWallpaper.qml"
                category: "Video Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("3D Engine Wallpaper (Godot 4.2)")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/GodotWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: true
            }

            ListElement {
                headline: qsTr("QML Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/QMLWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("HTML5 Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/HTMLWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("Website Wallpaper")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/WebsiteWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("QML Widget")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/QMLWidget.qml"
                category: "Widgets"
                proFeature: false
            }

            ListElement {
                headline: qsTr("HTML Widget")
                source: "qrc:/qml/ScreenPlayApp/qml/Create/Wizards/HTMLWidget.qml"
                category: "Widgets"
                proFeature: false
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AsNeeded
        }

        section.delegate: Item {
            height: headline.contentHeight + 20

            Text {
                id: headline
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
                if (proFeature && App.globalVariables.isBasicVersion())
                    return screenPlayProView.open()

                listView.currentIndex = index
                const item = stackView.push(source)
                loaderConnections.target = item
            }
            ToolButton {
                enabled: false
                visible: proFeature  && App.globalVariables.isBasicVersion()
                icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/font-awsome/lock-solid.svg"
                icon.width: 10
                icon.height: 10
                icon.color: "gold"
                anchors {
                    top: listItem.top
                    topMargin: -10
                    right: listItem.right
                    rightMargin: -10
                }
            }
        }
    }
    ScreenPlayProPopup {
        id: screenPlayProView
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
