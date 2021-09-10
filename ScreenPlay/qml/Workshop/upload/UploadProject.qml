import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Workshop 1.0
import ScreenPlay 1.0

Popup {
    id: root

    property ScreenPlayWorkshop screenPlayWorkshop
    property SteamWorkshop steam: screenPlayWorkshop.steamWorkshop

    width: 1200
    height: 700
    modal: true
    dim: true
    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.NoAutoClose
    onAboutToShow: uploadLoader.sourceComponent = com
    onAboutToHide: uploadLoader.sourceComponent = undefined

    Loader {
        id: uploadLoader

        anchors.fill: parent
    }

    Connections {
        function onRequestClosePopup() {
            root.close()
        }

        target: uploadLoader.item
    }

    Component {
        id: com

        Item {
            id: wrapper

            signal requestClosePopup

            Item {
                id: headerWrapper

                height: 50

                anchors {
                    top: parent.top
                    right: parent.right
                    left: parent.left
                    margins: 10
                }

                Text {
                    id: txtHeadline

                    text: qsTr("Upload Wallpaper/Widgets to Steam")
                    color: Material.foreground
                    font.pointSize: 21
                    font.family: ScreenPlay.settings.font
                    font.weight: Font.Thin

                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            SwipeView {
                id: view

                clip: true
                currentIndex: 0
                interactive: false

                anchors {
                    top: headerWrapper.bottom
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.left
                    margins: 10
                }

                Item {
                    id: firstPage

                    GridView {
                        id: gridView

                        boundsBehavior: Flickable.DragOverBounds
                        maximumFlickVelocity: 7000
                        flickDeceleration: 5000
                        cellWidth: parent.width
                        cellHeight: 250
                        clip: true
                        model: screenPlayWorkshop.installedListModel

                        anchors {
                            top: parent.top
                            right: parent.right
                            bottom: btnAbort.top
                            left: parent.left
                            margins: 10
                        }

                        delegate: UploadProjectBigItem {
                            id: delegate

                            focus: true
                            width: gridView.cellWidth - 30
                            customTitle: m_title
                            type: m_type
                            screenId: m_folderId
                            absoluteStoragePath: m_absoluteStoragePath
                            publishedFileID: m_publishedFileID
                            preview: m_preview
                            itemIndex: index
                            onItemClicked: {
                                for (let childItem in gridView.contentItem.children) {
                                    if (gridView.contentItem.children[childItem].isSelected) {
                                        btnUploadProjects.enabled = true
                                        return
                                    }
                                }
                                btnUploadProjects.enabled = false
                            }
                        }

                        ScrollBar.vertical: ScrollBar {
                            snapMode: ScrollBar.SnapOnRelease
                            policy: ScrollBar.AlwaysOn
                        }
                    }

                    Button {
                        id: btnAbort

                        text: qsTr("Abort")
                        onClicked: {
                            wrapper.requestClosePopup()
                        }

                        anchors {
                            right: btnUploadProjects.left
                            bottom: parent.bottom
                            margins: 10
                        }
                    }

                    Button {
                        id: btnUploadProjects

                        text: qsTr("Upload Selected Projects")
                        highlighted: true
                        enabled: false
                        onClicked: {
                            var uploadListArray = []
                            for (let childItem in gridView.contentItem.children) {
                                if (gridView.contentItem.children[childItem].isSelected)
                                    uploadListArray.push(
                                                gridView.contentItem.children[childItem].absoluteStoragePath)
                            }
                            view.currentIndex = 1
                            steam.bulkUploadToWorkshop(uploadListArray)
                        }

                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: 10
                        }
                    }
                }

                Item {
                    id: secondPage

                    ListView {
                        id: listView

                        boundsBehavior: Flickable.DragOverBounds
                        maximumFlickVelocity: 7000
                        flickDeceleration: 5000
                        cacheBuffer: 1000
                        clip: true
                        model: steam.uploadListModel
                        width: parent.width - 50
                        spacing: 25

                        anchors {
                            top: parent.top
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                            margins: 10
                        }

                        delegate: UploadProjectItem {
                            previewImagePath: m_absolutePreviewImagePath
                            progress: m_uploadProgress
                            name: m_name
                            steamStatus: m_status
                        }

                        ScrollBar.vertical: ScrollBar {
                            snapMode: ScrollBar.SnapOnRelease
                        }
                    }

                    Button {
                        id: btnFinish

                        text: qsTr("Finish")
                        highlighted: true
                        enabled: false
                        onClicked: {
                            root.close()
                        }

                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: 10
                        }

                        Connections {
                            function onUploadCompleted() {
                                btnFinish.enabled = true
                            }

                            target: steam.uploadListModel
                        }
                    }
                }
            }

            PageIndicator {
                id: indicator

                count: view.count
                currentIndex: view.currentIndex
                anchors.bottom: view.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Material.background
    }
}
