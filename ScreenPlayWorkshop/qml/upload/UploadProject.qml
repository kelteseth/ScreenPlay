import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop

Item {
    id: root
    objectName: "uploadProjectPage"

    property ScreenPlayWorkshop screenPlayWorkshop
    property SteamWorkshop steamWorkshop
    property StackView stackView

    // Selection model to track selected items across delegate recycling
    property var selectedItems: ({})

    // Reset state when page becomes visible
    Component.onCompleted: resetState()

    function resetState(): void {
        selectedItems = {}
        btnUploadProjects.enabled = false
        btnFinish.enabled = false
        view.currentIndex = 0
    }

    function toggleSelection(absoluteStoragePath: string, selected: bool): void {
        if (selected) {
            selectedItems[absoluteStoragePath] = true
        } else {
            delete selectedItems[absoluteStoragePath]
        }
        selectedItems = selectedItems // Trigger binding update
        btnUploadProjects.enabled = Object.keys(selectedItems).length > 0
    }

    function isItemSelected(absoluteStoragePath: string): bool {
        return absoluteStoragePath in selectedItems
    }

    Item {
        id: headerWrapper

        height: 60

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            topMargin: 20
            leftMargin: 10
            rightMargin: 10
        }

        Text {
            id: txtHeadline

            text: qsTr("Upload Wallpaper/Widgets to Steam")
            color: Material.foreground
            font.pointSize: 21

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
                objectName: "uploadGridView"

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
                    folderName: m_folderName
                    absoluteStoragePath: m_absoluteStoragePath
                    publishedFileID: m_publishedFileID
                    preview: m_preview
                    itemIndex: index
                    // Restore selection state when delegate is recycled
                    isSelected: root.isItemSelected(m_absoluteStoragePath)
                    onItemClicked: (folderName, type, isActive) => {
                        root.toggleSelection(m_absoluteStoragePath, isActive)
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                    policy: ScrollBar.AlwaysOn
                }
            }

            Button {
                id: btnAbort
                objectName: "btnAbort"

                text: qsTr("Abort")
                onClicked: {
                    stackView.pop()
                }

                anchors {
                    right: btnUploadProjects.left
                    bottom: parent.bottom
                    margins: 10
                }
            }

            Button {
                id: btnUploadProjects
                objectName: "btnUploadProjects"

                text: qsTr("Upload Selected Projects")
                highlighted: true
                enabled: false
                onClicked: {
                    // Disable immediately to prevent double-clicks
                    btnUploadProjects.enabled = false
                    // Use the selection model instead of iterating over visible delegates
                    const uploadListArray = Object.keys(root.selectedItems)
                    view.currentIndex = 1
                    root.steamWorkshop.bulkUploadToWorkshop(uploadListArray)
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
                model: root.steamWorkshop.uploadListModel
                spacing: 25

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottom: btnFinish.top
                    margins: 25
                }

                delegate: UploadProjectItem {
                    width: listView.width
                    previewImagePath: m_absolutePreviewImagePath
                    progress: m_uploadProgress
                    name: m_name
                    steamStatus: m_status
                    uploadState: m_uploadState
                }

                ScrollBar.vertical: ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                }
            }

            Button {
                id: btnFinish
                objectName: "btnFinish"

                text: qsTr("Finish")
                highlighted: true
                enabled: false
                onClicked: {
                    root.steamWorkshop.uploadListModel.clearWhenFinished()
                    root.resetState()
                    root.stackView.pop()
                }

                anchors {
                    right: listView.right
                    bottom: parent.bottom
                    bottomMargin: 25
                }

                Connections {
                    function onUploadCompleted() {
                        btnFinish.enabled = true
                    }

                    target: root.steamWorkshop.uploadListModel
                }
            }
        }
    }
}
