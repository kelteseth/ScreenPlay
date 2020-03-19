import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import ScreenPlay 1.0

Item {
    id: pageInstalled
    state: "out"

    signal setNavigationItem(var pos)
    signal setSidebarActive(var active)

    property bool refresh: false
    property bool enabled: true

    Component.onCompleted: {
        pageInstalled.state = "in"
        ScreenPlay.installedListFilter.sortByRoleType("All")
        checkIsContentInstalled()
    }

    Action {
        shortcut: "F5"
        onTriggered: ScreenPlay.installedListModel.reset()
    }

    Connections {
        target: loaderHelp.item
        onHelperButtonPressed: {
            setNavigationItem(pos)
        }
    }
    Connections {
        target: ScreenPlay.installedListModel
        onInstalledLoadingFinished: {
            checkIsContentInstalled()
        }
        onCountChanged: {
            if (count === 0) {
                checkIsContentInstalled()
            }
        }
    }

    function checkIsContentInstalled() {
        if (ScreenPlay.installedListModel.count === 0) {
            loaderHelp.active = true
            gridView.footerItem.isVisible = true
            gridView.visible = false
            navWrapper.visible = false
        } else {
            loaderHelp.active = false
            gridView.footerItem.isVisible = false
            refresh = false
            gridView.contentY = -82
            gridView.visible = true
            navWrapper.visible = true
        }
    }

    Loader {
        id: loaderHelp
        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledWelcomeScreen.qml"
    }

    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 7000
        flickDeceleration: 5000
        anchors.fill: parent
        cellWidth: 340
        cellHeight: 200
        interactive: pageInstalled.enabled
        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }
        header: Item {
            height: 82
            width: parent.width
            property bool isVisible: false
            opacity: 0
            onIsVisibleChanged: {
                if (isVisible) {
                    txtHeader.color =  Material.accent
                    txtHeader.text = qsTr("Refreshing!")
                } else {
                    txtHeader.color = "gray"
                    txtHeader.text = qsTr("Pull to refresh!")
                }
            }

            Timer {
                interval: 150
                running: true
                onTriggered: {
                    animFadeIn.start()
                }
            }

            PropertyAnimation on opacity {
                id: animFadeIn
                from: 0
                to: 1
                running: false
                duration: 1000
            }

            Text {
                id: txtHeader
                text: qsTr("Pull to refresh!")
                font.family: ScreenPlay.settings.font
                anchors.centerIn: parent
                color: "gray"
                font.pointSize: 18
            }
        }
        footer: Item {
            property bool isVisible: true
            height: 100
            opacity: 0
            visible: isVisible
            width: parent.width

            Text {
                id: txtFooter
                font.family: ScreenPlay.settings.font
                text: qsTr("Get more Wallpaper & Widgets via the Steam workshop!")
                anchors.centerIn: parent
                color: "gray"

                Timer {
                    interval: 400
                    running: true
                    onTriggered: {
                        animFadeInTxtFooter.start()
                    }
                }

                PropertyAnimation on opacity {
                    id: animFadeInTxtFooter
                    from: 0
                    to: 1
                    running: false
                    duration: 1000
                }
            }
        }
        property bool isDragging: false
        onDragStarted: isDragging = true
        onDragEnded: isDragging = false
        onContentYChanged: {
            if (contentY <= -180) {
                gridView.headerItem.isVisible = true
            } else {
                gridView.headerItem.isVisible = false
            }

            //Pull to refresh
            if (contentY <= -180 && !refresh && !isDragging) {
                ScreenPlay.installedListModel.reset()
            }
        }

        model: ScreenPlay.installedListFilter

        delegate: ScreenPlayItem {
            id: delegate
            focus: true

            customTitle: screenTitle
            type: screenType
            screenId: screenFolderId
            absoluteStoragePath: screenAbsoluteStoragePath
            workshopID: screenWorkshopID
            itemIndex: index
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }

    Item {
        id: navWrapper
        height: 115
        z: 999
        width: parent.width
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }

        RectangularGlow {
            id: effect
            anchors {
                fill: nav
            }
            cached: true
            glowRadius: 2
            spread: 0.15
            color: "black"
            opacity: 0.3
            cornerRadius: 15
        }

        Rectangle {
            id: nav
            color: Material.theme === Material.Light ? "white" : Material.background
            height: 50
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            MouseArea {
                anchors.fill: parent
            }
            TabBar {
                height: parent.height
                background: Item {}
                anchors {
                    top: parent.top
                    topMargin: 5
                    left: parent.left
                    leftMargin: 20
                    bottom: parent.bottom
                }

                TabButton {
                    text: qsTr("All")
                    font.family: ScreenPlay.settings.font
                    icon.height: 16
                    icon.width: 16
                    height: parent.height

                    width: implicitWidth
                    background: Item {}
                    icon.source: "qrc:/assets/icons/icon_installed.svg"

                    onClicked: {
                        setSidebarActive(false)
                        ScreenPlay.installedListFilter.sortByRoleType("All")
                    }
                }
                TabButton {
                    text: qsTr("Scenes")
                    icon.height: 16
                    icon.width: 16
                    font.family: ScreenPlay.settings.font
                    width: implicitWidth
                    height: parent.height
                    background: Item {}
                    icon.source: "qrc:/assets/icons/icon_code.svg"
                    onClicked: {
                        setSidebarActive(false)
                        ScreenPlay.installedListFilter.sortByRoleType("Scenes")
                    }
                }
                TabButton {
                    text: qsTr("Videos")
                    icon.height: 16
                    icon.width: 16
                    font.family: ScreenPlay.settings.font
                    height: parent.height
                    width: implicitWidth
                    background: Item {}
                    icon.source: "qrc:/assets/icons/icon_movie.svg"
                    onClicked: {
                        setSidebarActive(false)
                        ScreenPlay.installedListFilter.sortByRoleType("Videos")
                    }
                }
                TabButton {
                    text: qsTr("Widgets")
                    icon.height: 16
                    icon.width: 16
                    font.family: ScreenPlay.settings.font
                    height: parent.height
                    width: implicitWidth
                    background: Item {}
                    icon.source: "qrc:/assets/icons/icon_widgets.svg"
                    onClicked: {
                        setSidebarActive(false)
                        ScreenPlay.installedListFilter.sortByRoleType("Widgets")
                    }
                }
            }

            Image {
                id: icnSearch
                source: "qrc:/assets/icons/icon_search.svg"
                height: 15
                width: 15
                sourceSize: Qt.size(15, 15)
                anchors {
                    right: parent.right
                    rightMargin: 30
                    bottom: parent.bottom
                    bottomMargin: 15
                }
            }
            TextField {
                id: txtSearch
                width: 250
                height: 40
                font.family: ScreenPlay.settings.font
                leftPadding: 10
                anchors {
                    right: icnSearch.right
                    rightMargin: 20
                    top: parent.top
                    topMargin: 10
                }
                onTextChanged: {
                    if (txtSearch.text.length === 0) {
                        ScreenPlay.installedListFilter.resetFilter()
                    } else {
                        ScreenPlay.installedListFilter.sortByName(
                                    txtSearch.text)
                    }
                }

                selectByMouse: true
                placeholderText: qsTr("Search for Wallpaper & Widgets")
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: navWrapper
                anchors.topMargin: -115
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: navWrapper
                anchors.topMargin: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "out"
            to: "in"

            NumberAnimation {
                target: navWrapper
                property: "anchors.topMargin"
                duration: 400
                easing.type: Easing.InOutQuart
            }
        }
    ]
}
