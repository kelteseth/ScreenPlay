import QtQuick 2.9
import QtQml.Models 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Item {
    id: pageInstalled

    signal setSidebaractiveItem(var screenId, var type)
    signal setNavigationItem(var pos)

    property bool refresh: false

    Connections {
        target: loaderHelp.item
        onHelperButtonPressed: {
            setNavigationItem(pos)
        }
    }
    Connections {
        target: installedListModel
        onInstalledLoadingFinished: {
            refresh = false
            if (installedListModel.getAmountItemLoaded() === 0) {
                loaderHelp.active = true
            } else {
                loaderHelp.active = false
            }
        }
    }

    Component.onCompleted: {
        installedListModel.reset()
        installedListModel.loadScreens()
    }

    Loader {
        id: loaderHelp
        asynchronous: true
        active: false
        z: 99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledUserHelper.qml"
    }

    GridView {
        id: gridView
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 7000
        flickDeceleration: 5000
        anchors.fill: parent
        cellWidth: 340
        cacheBuffer: 10000
        cellHeight: 200

        //        onContentYChanged: {
        //            //Pull to refresh
        //            if (contentY <= -120 && !refresh) {
        //                refresh = true
        //                installedListModel.reset()
        //                installedListModel.loadScreens()
        //            }
        //        }
        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }
        header: Item {
            height: 30
            width: parent.width
        }

        model: installedListFilter

        delegate: ScreenPlayItem {
            id: delegate
            focus: true

            customTitle: screenTitle
            type: screenType
            screenId: screenFolderId
            absoluteStoragePath: screenAbsoluteStoragePath

            Connections {
                target: delegate
                onItemClicked: {
                    setSidebaractiveItem(screenId, type)
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }
    function onPageChanged(name) {
        if (name === "All") {
            installedListFilter.sortByRoleType("All")
            navAll.state = "active"
            navWallpaper.state = "inactive"
            navWidgets.state = "inactive"
        } else if (name === "Wallpaper") {
            installedListFilter.sortByRoleType("Wallpaper")
            navAll.state = "inactive"
            navWallpaper.state = "active"
            navWidgets.state = "inactive"
        } else if (name === "Widgets") {
            installedListFilter.sortByRoleType("Widgets")
            navAll.state = "inactive"
            navWallpaper.state = "inactive"
            navWidgets.state = "active"
        }
    }
    Item {
        id: navWrapper
        height: 120
        z: 999
        width: parent.width
        anchors {
            bottom: parent.bottom
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
            height: 65
            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
            }

            Row {
                anchors {
                    top: parent.top
                    right: txtSearch.left
                    left: parent.left
                    leftMargin: 20
                    bottom: parent.bottom
                    bottomMargin: -5
                }

                spacing: 0

                InstalledNavigation {
                    id: navAll
                    state: "active"
                    name: qsTr("All")
                    iconSource: "qrc:/assets/icons/icon_installed.svg"
                    onPageClicked: {
                        onPageChanged(name)
                    }
                }
                InstalledNavigation {
                    id: navWallpaper
                    state: "inactive"
                    name: qsTr("Wallpaper")
                    iconSource: "qrc:/assets/icons/icon_movie.svg"
                    onPageClicked: {
                        onPageChanged(name)
                    }
                }
                InstalledNavigation {
                    id: navWidgets
                    state: "inactive"
                    name: qsTr("Widgets")
                    iconSource: "qrc:/assets/icons/icon_widgets.svg"
                    onPageClicked: {
                        onPageChanged(name)
                    }
                }
            }
            TextField {
                id: txtSearch
                width: 300
                anchors {
                    right: parent.right
                    rightMargin: 30
                    bottom: parent.bottom
                    bottomMargin: 10
                }
                onTextChanged: {
                    if (txtSearch.text.length === 0) {
                        installedListFilter.resetFilter()
                    } else {
                        installedListFilter.sortByName(txtSearch.text)
                    }
                }

                selectByMouse: true
                text: qsTr("")
                placeholderText: "Search for Wallpaper & Widgets"
            }
        }
    }
}
