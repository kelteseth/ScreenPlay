import QtQuick 2.9
import QtQml.Models 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4

Item {
    id: pageInstalled


    signal setSidebaractiveItem(var screenId)
    signal setNavigationItem(var pos)


    Connections {
        target:loaderHelp.item
        onHelperButtonPressed:{
            setNavigationItem(pos)
        }
    }
    Connections {
        target: installedListModel
        onInstalledLoadingFinished:{
            if(installedListModel.getAmountItemLoaded() === 0){
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
        id:loaderHelp
        asynchronous: true
        active:false
        z:99
        anchors.fill: parent
        source: "qrc:/qml/Installed/InstalledUserHelper.qml"
    }

    Button {
        z:111
        visible: false
        anchors {
            top: parent.top
            right: parent.right
        }
        text: qsTr("Reload")
        onClicked: {
            installedListModel.reloadFiles()

        }
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
        anchors {
            topMargin: 0
            rightMargin: 0
            leftMargin: 30
        }
        header: Item {
            height: 30
            width: parent.width
        }
        model: installedListModel

        delegate: ScreenPlayItem {
            id: delegate
            focus: true

            customTitle: screenTitle
            screenId: screenFolderId
            absoluteStoragePath: screenAbsoluteStoragePath

            Connections {
                target: delegate
                onItemClicked: {
                    setSidebaractiveItem(screenId)
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }
}
