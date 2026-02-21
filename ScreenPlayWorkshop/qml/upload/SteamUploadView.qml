import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop
import ScreenPlayCore as SPCore

/*!
    \qmltype SteamUploadView
    \brief Upload page for publishing local wallpapers/widgets to the Steam Workshop.

    Shows a two-column grid of locally installed items that are not yet on the
    Workshop.  The user selects one or more and submits them for upload.
*/
Item {
    id: root
    objectName: "uploadProjectPage"

    required property ScreenPlayWorkshop screenPlayWorkshop
    required property SteamWorkshop steamWorkshop
    required property StackView stackView

    // Selection model – tracks absoluteStoragePath → true across delegate recycling
    property var selectedItems: ({})
    property int selectionCount: 0
    property bool ascending: false

    focus: true
    Keys.onEscapePressed: root.stackView.pop()
    TapHandler {
        acceptedButtons: Qt.BackButton
        onTapped: root.stackView.pop()
    }

    Component.onCompleted: {
        resetState();
        // Default: sort by Date Modified, most recent first (descending)
        root.screenPlayWorkshop.installedListFilter.sort(1, false)
    }

    function resetState(): void {
        selectedItems = {}
        selectionCount = 0
        view.currentIndex = 0
    }

    function toggleSelection(absoluteStoragePath: string, selected: bool): void {
        if (selected) {
            selectedItems[absoluteStoragePath] = true
        } else {
            delete selectedItems[absoluteStoragePath]
        }
        selectionCount = Object.keys(selectedItems).length
    }

    function isItemSelected(absoluteStoragePath: string): bool {
        return absoluteStoragePath in selectedItems
    }

    SwipeView {
        id: view
        anchors.fill: parent
        clip: true
        currentIndex: 0
        interactive: false

        // ── Page 1: Grid selection ──────────────────────────────────────
        Item {
            id: firstPage

            // Full-page background image used by ImageBlurContainer for blur.
            // Loads the first item's preview; hidden (only a shader source).
            Image {
                id: backgroundImage
                anchors.fill: parent
                visible: false
                fillMode: Image.PreserveAspectCrop

                Connections {
                    target: root.screenPlayWorkshop.installedListModel
                    function onInstalledLoadingFinished(): void {
                        const mdl = root.screenPlayWorkshop.installedListModel
                        if (mdl.rowCount() === 0)
                            return
                        const idx = mdl.index(0, 0)
                        const storagePath = mdl.data(idx, 263)
                        // AbsoluteStoragePath
                        const preview = mdl.data(idx, 258)
                        // Preview
                        if (storagePath && preview)
                            backgroundImage.source = Qt.resolvedUrl(storagePath + "/" + preview)
                    }
                }
            }

            SPCore.MaterialGridView {
                id: gridView
                objectName: "uploadGridView"

                cellWidth: Math.max(330, (gridView.width - gridView.anchors.leftMargin) / 2)
                cellHeight: 160
                clip: true
                boundsBehavior: Flickable.DragOverBounds
                model: root.screenPlayWorkshop.installedListFilter

                anchors {
                    fill: parent
                    leftMargin: 45
                }

                header: Item {
                    id: headerItem

                    height: 90 + 20
                    width: gridView.width - gridView.anchors.leftMargin

                    SPCore.ImageBlurContainer {
                        id: headerBar
                        backgroundSource: backgroundImage
                        flickable: gridView
                        stackView: root.stackView
                        width: parent.width
                        height: 70
                        radius: 8
                        clip: true

                        anchors {
                            top: parent.top
                            topMargin: 20
                            left: parent.left
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 12

                            Button {
                                Layout.preferredWidth: implicitWidth
                                icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_left.svg"
                                flat: true
                                onClicked: root.stackView.pop()
                                ToolTip.visible: hovered
                                ToolTip.text: qsTr("Back to Workshop")
                            }

                            SteamImage {
                                id: avatar
                                Layout.preferredWidth: 55
                                Layout.preferredHeight: 55
                                Component.onCompleted: {
                                    root.steamWorkshop.steamAccount.loadAvatar()
                                }
                                Connections {
                                    function onAvatarChanged(_avatar: var): void {
                                        avatar.setImage(_avatar)
                                    }
                                    target: root.steamWorkshop.steamAccount
                                }
                            }

                            ColumnLayout {
                                spacing: 0

                                Label {
                                    text: qsTr("Upload to Workshop")
                                    font.pointSize: 14
                                    font.bold: true
                                    color: "white"
                                }

                                Label {
                                    text: root.selectionCount > 0 ? qsTr("%1 selected").arg(root.selectionCount) : qsTr("Select items to upload")
                                    font.pointSize: 11
                                    color: Qt.rgba(1, 1, 1, 0.7)
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            ComboBox {
                                id: cbSort
                                Layout.preferredWidth: 180
                                Layout.alignment: Qt.AlignVCenter
                                textRole: "text"
                                valueRole: "value"
                                currentIndex: 1
                                model: [
                                    {
                                        "value": 0,
                                        "text": qsTr("Name")
                                    },
                                    {
                                        "value": 1,
                                        "text": qsTr("Date Modified")
                                    }
                                ]
                                onActivated: {
                                    root.screenPlayWorkshop.installedListFilter.sort(cbSort.currentValue, root.ascending)
                                }
                            }

                            Button {
                                id: btnSortOrder
                                flat: true
                                icon.source: root.ascending ? "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_sort-up-solid.svg" : "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_sort-down-solid.svg"
                                ToolTip.visible: hovered
                                ToolTip.text: root.ascending ? qsTr("Ascending") : qsTr("Descending")
                                onClicked: {
                                    root.ascending = !root.ascending
                                    root.screenPlayWorkshop.installedListFilter.sort(cbSort.currentValue, root.ascending)
                                }
                            }

                            Button {
                                objectName: "btnUploadProjects"
                                text: qsTr("Upload")
                                icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_file_upload.svg"
                                highlighted: true
                                enabled: root.selectionCount > 0
                                onClicked: {
                                    const uploadListArray = Object.keys(root.selectedItems)
                                    view.currentIndex = 1
                                    root.steamWorkshop.bulkUploadToWorkshop(uploadListArray)
                                }
                            }
                        }
                    }
                }

                delegate: UploadProjectBigItem {
                    id: delegate

                    height: gridView.cellHeight - 10
                    width: gridView.cellWidth - 10

                    focus: true
                    customTitle: m_title
                    type: m_type
                    folderName: m_folderName
                    absoluteStoragePath: m_absoluteStoragePath
                    publishedFileID: m_publishedFileID
                    preview: m_preview
                    itemIndex: index
                    isSelected: root.isItemSelected(m_absoluteStoragePath)
                    onItemClicked: (folderName, type, isActive) => {
                        root.toggleSelection(m_absoluteStoragePath, isActive)
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    snapMode: ScrollBar.SnapOnRelease
                    policy: ScrollBar.AsNeeded
                }
            }
        }

        // ── Page 2: Upload progress ─────────────────────────────────────
        Item {
            id: secondPage

            ListView {
                id: listView
                objectName: "uploadListView"

                boundsBehavior: Flickable.DragOverBounds
                maximumFlickVelocity: 7000
                flickDeceleration: 5000
                cacheBuffer: 1000
                clip: true
                model: root.steamWorkshop.uploadListModel
                spacing: 20

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottom: btnFinish.top
                    leftMargin: 60
                    rightMargin: 60
                    topMargin: 25
                    bottomMargin: 25
                }

                header: Item {
                    width: listView.width
                    height: 100

                    Label {
                        anchors.centerIn: parent
                        text: qsTr("Uploading to Steam Workshop")
                        font.pointSize: 16
                        font.weight: Font.Normal
                        color: Material.foreground
                        opacity: 0.8
                    }
                }

                delegate: UploadProjectItem {
                    width: Math.min(listView.width, 800)
                    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
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
                    function onUploadCompleted(): void {
                        btnFinish.enabled = true
                    }
                    target: root.steamWorkshop.uploadListModel
                }
            }
        }
    }
}
