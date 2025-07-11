pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlay
import ScreenPlayCore
import "../Components"

Rectangle {
    id: root
    objectName: "createSidebar"

    property bool expanded: false
    property alias listView: listView
    property alias model: listView.model
    property StackView stackView
    property Item modalSource

    width: 320
    state: expanded ? "" : "inactive"
    layer.enabled: true
    Component.onCompleted: expanded = true
    color: Material.background

    ListView {
        id: listView
        objectName: "wizardsListView"

        anchors.fill: parent
        anchors.margins: 20
        anchors.rightMargin: 0
        spacing: 2
        section.property: "category"

        Connections {
            id: loaderConnections

            function onWizardStarted() {
                root.expanded = false;
            }

            function onWizardExited() {
                root.expanded = true;

                App.uiAppStateSignals.setNavigation("Installed");
                App.uiAppStateSignals.setNavigationActive(true);
            }

            ignoreUnknownSignals: true
        }

        model: ListModel {

            ListElement {
                headline: qsTr("Example Content")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/ExampleContent.qml"
                category: "Home"
                proFeature: false
            }
            ListElement {
                headline: qsTr("Tools Overview")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/StartInfo.qml"
                category: "Home"
                proFeature: false
            }

            ListElement {
                headline: qsTr("Import Video")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/ImportVideoAndConvert/CreateWallpaper.qml"
                category: "Video Wallpaper"
                objectName: "videoImportConvert"
                proFeature: false
            }

            ListElement {
                headline: qsTr("GIF Wallpaper")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/GifWallpaper.qml"
                category: "Video Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("🚀 3D Engine Wallpaper (Godot 4.4)")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/GodotWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: true
            }

            ListElement {
                headline: qsTr("QML Wallpaper")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/QMLWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("HTML5 Wallpaper")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/HTMLWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("Website Wallpaper")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/WebsiteWallpaper.qml"
                category: "3D Engine & \nCode Wallpaper"
                proFeature: false
            }

            ListElement {
                headline: qsTr("QML Widget")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/QMLWidget.qml"
                category: "Widgets"
                proFeature: false
            }

            ListElement {
                headline: qsTr("HTML Widget")
                source: "qrc:/qt/qml/ScreenPlay/qml/Create/Wizards/HTMLWidget.qml"
                category: "Widgets"
                proFeature: false
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AsNeeded
        }

        section.delegate: Item {
            id: sectionHeadline
            height: headline.contentHeight + 20
            required property string section

            Text {
                id: headline
                text: sectionHeadline.section
                font.pointSize: 18
                color: Material.primaryTextColor

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    bottomMargin: 10
                }
            }
        }

        delegate: Button {
            id: listItem

            required property string headline
            required property bool proFeature
            required property string source
            required property string objectName
            required property int index

            width: listView.width - 40
            height: 45
            highlighted: ListView.isCurrentItem
            text: headline
            onClicked: {
                if (listItem.proFeature && App.globalVariables.isBasicVersion())
                    return screenPlayProView.open();
                listView.currentIndex = index;
                const item = root.stackView.push(source);
                loaderConnections.target = item;
            }

            ToolButton {
                enabled: false
                visible: listItem.proFeature && App.globalVariables.isBasicVersion()
                icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/font-awsome/lock-solid.svg"
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
        modalSource: root.modalSource
    }
    layer.effect: ElevationEffect {
        elevation: 6
    }

    states: [
        State {
            name: ""

            PropertyChanges {
                root.anchors.leftMargin: 0
                root.opacity: 1
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                root.opacity: 0
                root.anchors.leftMargin: -root.width
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
