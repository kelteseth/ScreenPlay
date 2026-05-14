import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop
import ScreenPlayCore as SPCore

/*!
    \qmltype SteamProfileWorkshopItem
    \brief A detailed view page for a user's Steam Workshop item with edit capabilities.

    Displays comprehensive information about a workshop item including
    statistics, metadata, and provides controls for editing title, description,
    tags, visibility, and deleting the item.
*/
Item {
    id: root
    objectName: "SteamProfileWorkshopItemPage"

    required property StackView stackView
    required property SteamWorkshop steamWorkshop
    required property var publishedFileID
    required property url previewImageUrl

    focus: true
    Keys.onEscapePressed: root.stackView.pop()
    // Mouse back button is a pointer event, not a key event, so TapHandler is needed
    TapHandler {
        acceptedButtons: Qt.BackButton
        onTapped: root.stackView.pop()
    }

    property string itemTitle: ""
    property string itemDescription: ""
    property var itemTags: []
    property int votesUp: 0
    property int votesDown: 0
    property real score: 0
    property string itemUrl: ""
    property int fileSize: 0
    property int totalFileSize: 0
    property int timeCreated: 0
    property int timeUpdated: 0
    property int visibility: 0
    property bool banned: false
    property bool acceptedForUse: false
    property int subscriptionCount: 0
    property int favoriteCount: 0
    property int followerCount: 0
    property int uniqueWebsiteViews: 0
    property int numChildren: 0
    property bool isLoading: true

    property bool isSaving: false
    property bool isDeleting: false
    property string editTitle: ""
    property string editDescription: ""
    property var editTags: []
    property int editVisibility: 0

    property bool isSubscribed: false
    property bool isContentUpdating: false
    property real contentUpdateProgress: 0
    property int contentUpdateStatus: 0
    property var itemFiles: []
    property string installPath: ""

    readonly property bool isEditMode: root.state === "edit"

    state: "view"
    states: [
        State {
            name: "view"
        },
        State {
            name: "edit"
            PropertyChanges {
                root.editTitle: root.itemTitle
                root.editDescription: root.itemDescription
                root.editTags: root.itemTags.slice()
                root.editVisibility: root.visibility
            }
        }
    ]

    StackView.onActivated: {
        root.isLoading = true
        root.steamWorkshop.itemOps.requestProfileItemDetails(root.publishedFileID)
    }

    function saveChanges(): void {
        root.isSaving = true
        const vis = root.editVisibility !== root.visibility ? root.editVisibility : -1
        root.steamWorkshop.itemOps.updateItemMetadata(root.publishedFileID, root.editTitle, root.editDescription, root.editTags, vis)
    }

    function loadInstalledFiles(): void {
        const fileList = root.steamWorkshop.itemOps.getItemFileList(root.publishedFileID)
        root.itemFiles = fileList
        const installInfo = root.steamWorkshop.itemOps.getItemInstallInfo(root.publishedFileID)
        root.installPath = installInfo["path"] ?? ""
    }

    function getUpdateStatusText(status: int): string {
        switch (status) {
        case 0:
            return qsTr("Invalid")
        case 1:
            return qsTr("Preparing config...")
        case 2:
            return qsTr("Preparing content...")
        case 3:
            return qsTr("Uploading content...")
        case 4:
            return qsTr("Uploading preview...")
        case 5:
            return qsTr("Committing changes...")
        default:
            return qsTr("Updating...")
        }
    }

    function formatFileSize(bytes: int): string {
        if (bytes < 1024)
            return bytes + " B"
        if (bytes < 1024 * 1024)
            return Math.round(bytes / 1024) + " KB"
        return Math.round(bytes / (1024 * 1024)) + " MB"
    }

    function formatDate(unixTimestamp: int): string {
        return new Date(unixTimestamp * 1000).toLocaleDateString(Qt.locale(), Locale.ShortFormat)
    }

    function formatDateTime(unixTimestamp: int): string {
        return new Date(unixTimestamp * 1000).toLocaleString(Qt.locale(), Locale.ShortFormat)
    }

    function getVisibilityText(vis: int): string {
        switch (vis) {
        case 0:
            return qsTr("Public")
        case 1:
            return qsTr("Friends Only")
        case 2:
            return qsTr("Private")
        case 3:
            return qsTr("Unlisted")
        default:
            return qsTr("Unknown")
        }
    }

    Connections {
        target: root.steamWorkshop.itemOps

        function onRequestProfileItemDetailReturned(detail): void {
            if (detail.publishedFileId !== root.publishedFileID)
                return
            root.itemTitle = detail.title
            root.itemDescription = detail.description
            root.itemTags = detail.tags
            root.votesUp = detail.votesUp
            root.votesDown = detail.votesDown
            root.score = detail.score
            root.itemUrl = detail.url
            root.fileSize = detail.fileSize
            root.totalFileSize = detail.totalFileSize
            root.timeCreated = detail.timeCreated
            root.timeUpdated = detail.timeUpdated
            root.visibility = detail.visibility
            root.banned = detail.banned
            root.acceptedForUse = detail.acceptedForUse
            root.subscriptionCount = detail.subscriptionCount
            root.favoriteCount = detail.favoriteCount
            root.followerCount = detail.followerCount
            root.uniqueWebsiteViews = detail.uniqueWebsiteViews
            root.numChildren = detail.numChildren
            root.isLoading = false
            root.isSubscribed = root.steamWorkshop.itemOps.isSubscribed(root.publishedFileID)
            root.loadInstalledFiles()
        }

        function onWorkshopItemDeleted(success: bool, publishedFileID: var): void {
            if (publishedFileID !== root.publishedFileID)
                return
            root.isDeleting = false
            deleteConfirmDialog.close()
            if (success) {
                root.stackView.pop()
            }
        }

        function onWorkshopItemMetadataUpdated(success: bool, publishedFileID: var, eResult: int): void {
            if (publishedFileID !== root.publishedFileID)
                return
            root.isSaving = false
            if (success) {
                root.itemTitle = root.editTitle
                root.itemDescription = root.editDescription
                root.itemTags = root.editTags
                root.visibility = root.editVisibility
                root.state = "view"
            } else {
                errorDialog.errorCode = eResult
                errorDialog.open()
            }
        }

        function onWorkshopItemContentUpdated(success: bool, publishedFileID: var): void {
            if (publishedFileID !== root.publishedFileID)
                return
            root.isContentUpdating = false
            root.contentUpdateProgress = 0
            contentUpdateTimer.stop()
            if (success) {
                root.loadInstalledFiles()
            }
        }
    }

    Timer {
        id: contentUpdateTimer
        interval: 250
        repeat: true
        running: root.isContentUpdating
        onTriggered: {
            const info = root.steamWorkshop.itemOps.getContentUpdateProgress()
            root.contentUpdateProgress = info["progress"]
            root.contentUpdateStatus = info["status"]
        }
    }

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: root.previewImageUrl
        fillMode: Image.PreserveAspectCrop

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "transparent"
                }
                GradientStop {
                    position: 0.2
                    color: "transparent"
                }
                GradientStop {
                    position: 0.5
                    color: Qt.rgba(Material.backgroundColor.r, Material.backgroundColor.g, Material.backgroundColor.b, 0.85)
                }
                GradientStop {
                    position: 1.0
                    color: Material.backgroundColor
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: root.isLoading
        visible: root.isLoading
    }

    SPCore.ImageBlurContainer {
        id: headerBar
        backgroundSource: backgroundImage
        flickable: scrollView
        stackView: root.stackView
        width: parent.width - 90
        height: 70
        radius: 8
        clip: true
        visible: !root.isLoading

        anchors {
            top: parent.top
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
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
                ToolTip.text: qsTr("Back to Profile")
            }

            ColumnLayout {
                spacing: 0

                Label {
                    text: root.itemTitle
                    font.pointSize: 14
                    font.bold: true
                    color: "white"
                    elide: Text.ElideRight
                    Layout.maximumWidth: 400
                }

                Label {
                    text: qsTr("Published: %1 • Updated: %2").arg(root.formatDate(root.timeCreated)).arg(root.formatDate(root.timeUpdated))
                    font.pointSize: 10
                    color: Qt.rgba(1, 1, 1, 0.7)
                    visible: !root.isLoading
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Rectangle {
                visible: root.banned
                color: Material.color(Material.Red)
                radius: 4
                implicitWidth: bannedLabel.width + 16
                implicitHeight: 28

                Label {
                    id: bannedLabel
                    anchors.centerIn: parent
                    text: qsTr("BANNED")
                    color: "white"
                    font.bold: true
                    font.pointSize: 10
                }
            }

            Rectangle {
                visible: root.acceptedForUse
                color: Material.color(Material.Green)
                radius: 4
                implicitWidth: acceptedLabel.width + 16
                implicitHeight: 28

                Label {
                    id: acceptedLabel
                    anchors.centerIn: parent
                    text: qsTr("ACCEPTED")
                    color: "white"
                    font.bold: true
                    font.pointSize: 10
                }
            }

            Button {
                visible: !root.isEditMode
                text: qsTr("Edit")
                icon.source: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_edit.svg"
                highlighted: true
                onClicked: root.state = "edit"
            }

            Button {
                visible: root.isEditMode
                text: qsTr("Save")
                icon.source: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_done.svg"
                highlighted: true
                enabled: !root.isSaving
                onClicked: root.saveChanges()
            }

            Button {
                visible: root.isEditMode
                text: qsTr("Cancel")
                onClicked: root.state = "view"
            }

            BusyIndicator {
                visible: root.isSaving
                running: root.isSaving
                implicitWidth: 24
                implicitHeight: 24
            }

            Button {
                text: root.isSubscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                icon.source: root.isSubscribed ? "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg" : "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
                highlighted: !root.isSubscribed
                onClicked: {
                    if (root.isSubscribed) {
                        root.steamWorkshop.itemOps.unsubscribeItem(root.publishedFileID)
                    } else {
                        root.steamWorkshop.itemOps.subscribeItem(root.publishedFileID)
                    }
                    root.isSubscribed = !root.isSubscribed
                }
            }

            Button {
                visible: root.installPath !== ""
                text: qsTr("Open Folder")
                icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
                onClicked: Qt.openUrlExternally("file:///" + root.installPath)
            }

            Button {
                text: qsTr("Steam")
                icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_steam.svg"
                onClicked: Qt.openUrlExternally("steam://url/CommunityFilePage/" + root.publishedFileID)
            }

            Button {
                text: qsTr("Delete")
                icon.source: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_delete.svg"
                icon.color: "white"
                Material.background: Material.color(Material.Red)
                onClicked: deleteConfirmDialog.open()
            }
        }
    }

    Flickable {
        id: scrollView
        visible: !root.isLoading
        clip: true
        contentWidth: parent.width
        contentHeight: mainColumn.implicitHeight + 40

        anchors {
            top: headerBar.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        ColumnLayout {
            id: mainColumn
            width: parent.width
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 300

                ColumnLayout {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        bottomMargin: 20
                        leftMargin: 50
                        rightMargin: 75
                    }
                    spacing: 8

                    TextField {
                        visible: root.isEditMode
                        text: root.editTitle
                        onTextChanged: root.editTitle = text
                        font.pointSize: 20
                        font.bold: true
                        Layout.fillWidth: true
                        placeholderText: qsTr("Title")
                    }

                    Label {
                        visible: !root.isEditMode
                        text: root.itemTitle
                        font.pointSize: 24
                        font.bold: true
                        color: "white"
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: qsTr("Published: %1 • Updated: %2").arg(root.formatDate(root.timeCreated)).arg(root.formatDate(root.timeUpdated))
                        font.pointSize: 11
                        color: Qt.rgba(1, 1, 1, 0.8)
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.topMargin: 20
                Layout.bottomMargin: 20
                Layout.leftMargin: 50
                Layout.rightMargin: 75
                spacing: 16

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    StatCard {
                        title: qsTr("Subscriptions")
                        value: root.subscriptionCount.toLocaleString()
                        iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
                        Layout.fillWidth: true
                    }

                    StatCard {
                        title: qsTr("Favorites")
                        value: root.favoriteCount.toLocaleString()
                        iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_up.svg"
                        Layout.fillWidth: true
                    }

                    StatCard {
                        title: qsTr("Views")
                        value: root.uniqueWebsiteViews.toLocaleString()
                        iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
                        Layout.fillWidth: true
                    }

                    StatCard {
                        title: qsTr("Votes")
                        value: "👍 " + root.votesUp + "  👎 " + root.votesDown
                        iconSource: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_up.svg"
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    // Left column: Description + Details
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        spacing: 16

                        SPCore.ImageBlurContainer {
                            Layout.fillWidth: true
                            implicitHeight: descColumn.implicitHeight + 30
                            backgroundSource: backgroundImage
                            flickable: scrollView
                            stackView: root.stackView
                            radius: 8
                            visible: root.itemDescription !== "" || root.isEditMode

                            ColumnLayout {
                                id: descColumn
                                anchors {
                                    fill: parent
                                    margins: 15
                                }
                                spacing: 10

                                Label {
                                    text: qsTr("Description")
                                    font.pointSize: 14
                                    font.bold: true
                                    color: Material.foreground
                                }

                                TextArea {
                                    visible: root.isEditMode
                                    text: root.editDescription
                                    onTextChanged: root.editDescription = text
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                    Layout.minimumHeight: 100
                                    placeholderText: qsTr("Enter description...")
                                }

                                Label {
                                    visible: !root.isEditMode
                                    text: root.itemDescription || qsTr("No description")
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                    color: root.itemDescription ? Material.foreground : Material.secondaryTextColor
                                    font.pointSize: 11
                                }
                            }
                        }

                        SPCore.ImageBlurContainer {
                            Layout.fillWidth: true
                            implicitHeight: detailsColumn.implicitHeight + 30
                            backgroundSource: backgroundImage
                            flickable: scrollView
                            stackView: root.stackView
                            radius: 8

                            ColumnLayout {
                                id: detailsColumn
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: 15
                                }
                                spacing: 12

                                Label {
                                    text: qsTr("Details")
                                    font.pointSize: 14
                                    font.bold: true
                                    color: Material.foreground
                                }

                                GridLayout {
                                    columns: 2
                                    columnSpacing: 20
                                    rowSpacing: 8
                                    Layout.fillWidth: true

                                    Label {
                                        text: qsTr("File ID:")
                                        color: Material.secondaryTextColor
                                    }
                                    Label {
                                        text: root.publishedFileID
                                        color: Material.foreground
                                        Layout.fillWidth: true
                                    }

                                    Label {
                                        text: qsTr("File Size:")
                                        color: Material.secondaryTextColor
                                    }
                                    Label {
                                        text: root.formatFileSize(root.totalFileSize > 0 ? root.totalFileSize : root.fileSize)
                                        color: Material.foreground
                                    }

                                    Label {
                                        text: qsTr("Created:")
                                        color: Material.secondaryTextColor
                                    }
                                    Label {
                                        text: root.formatDateTime(root.timeCreated)
                                        color: Material.foreground
                                    }

                                    Label {
                                        text: qsTr("Updated:")
                                        color: Material.secondaryTextColor
                                    }
                                    Label {
                                        text: root.formatDateTime(root.timeUpdated)
                                        color: Material.foreground
                                    }

                                    Label {
                                        text: qsTr("Visibility:")
                                        color: Material.secondaryTextColor
                                    }
                                    RowLayout {
                                        ComboBox {
                                            visible: root.isEditMode
                                            model: [qsTr("Public"), qsTr("Friends Only"), qsTr("Private"), qsTr("Unlisted")]
                                            currentIndex: root.editVisibility
                                            onActivated: index => root.editVisibility = index
                                            Layout.preferredWidth: 140
                                        }
                                        Label {
                                            visible: !root.isEditMode
                                            text: root.getVisibilityText(root.visibility)
                                            color: Material.foreground
                                        }
                                    }

                                    Label {
                                        text: qsTr("Followers:")
                                        color: Material.secondaryTextColor
                                        visible: root.followerCount > 0
                                    }
                                    Label {
                                        text: root.followerCount.toLocaleString()
                                        color: Material.foreground
                                        visible: root.followerCount > 0
                                    }
                                }
                            }
                        }
                    }

                    // Right column: Tags + Files
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        spacing: 16

                        SPCore.ImageBlurContainer {
                            Layout.fillWidth: true
                            implicitHeight: tagsColumn.implicitHeight + 30
                            backgroundSource: backgroundImage
                            flickable: scrollView
                            stackView: root.stackView
                            radius: 8
                            visible: (root.itemTags.length > 0 && root.itemTags[0] !== "") || root.isEditMode

                            ColumnLayout {
                                id: tagsColumn
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: 15
                                }
                                spacing: 10

                                Label {
                                    text: qsTr("Tags")
                                    font.pointSize: 14
                                    font.bold: true
                                    color: Material.foreground
                                }

                                Flow {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Row {
                                        visible: root.isEditMode
                                        spacing: 8

                                        TextField {
                                            id: newTagField
                                            placeholderText: qsTr("New tag...")
                                            implicitWidth: 120
                                            implicitHeight: 28
                                            verticalAlignment: TextInput.AlignVCenter
                                            topPadding: 2
                                            bottomPadding: 2
                                            onAccepted: {
                                                if (text.trim() !== "") {
                                                    let tags = root.editTags.slice()
                                                    tags.push(text.trim())
                                                    root.editTags = tags
                                                    text = ""
                                                }
                                            }
                                        }

                                        Button {
                                            text: qsTr("+")
                                            implicitWidth: 32
                                            implicitHeight: 28
                                            topInset: 0
                                            bottomInset: 0
                                            enabled: newTagField.text.trim() !== ""
                                            onClicked: {
                                                if (newTagField.text.trim() !== "") {
                                                    let tags = root.editTags.slice()
                                                    tags.push(newTagField.text.trim())
                                                    root.editTags = tags
                                                    newTagField.text = ""
                                                }
                                            }
                                        }
                                    }

                                    Repeater {
                                        model: root.isEditMode ? root.editTags : root.itemTags

                                        delegate: Rectangle {
                                            id: tagDelegate
                                            required property string modelData
                                            required property int index

                                            visible: tagDelegate.modelData !== ""
                                            width: tagRow.width + (root.isEditMode ? 8 : 16)
                                            height: 28
                                            radius: 14
                                            color: Material.accent

                                            RowLayout {
                                                id: tagRow
                                                anchors.centerIn: parent
                                                spacing: 4

                                                Label {
                                                    text: tagDelegate.modelData
                                                    color: "white"
                                                    font.pointSize: 10
                                                }

                                                ToolButton {
                                                    visible: root.isEditMode
                                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
                                                    icon.width: 12
                                                    icon.height: 12
                                                    icon.color: "white"
                                                    implicitWidth: 20
                                                    implicitHeight: 20
                                                    onClicked: {
                                                        let tags = root.editTags.slice()
                                                        tags.splice(tagDelegate.index, 1)
                                                        root.editTags = tags
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        SPCore.ImageBlurContainer {
                            Layout.fillWidth: true
                            implicitHeight: filesColumn.implicitHeight + 30
                            backgroundSource: backgroundImage
                            flickable: scrollView
                            stackView: root.stackView
                            radius: 8

                            ColumnLayout {
                                id: filesColumn
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: 15
                                }
                                spacing: 10

                                RowLayout {
                                    Layout.fillWidth: true

                                    Label {
                                        text: qsTr("Files")
                                        font.pointSize: 14
                                        font.bold: true
                                        color: Material.foreground
                                        Layout.fillWidth: true
                                    }

                                    Label {
                                        visible: root.itemFiles.length > 0
                                        text: root.itemFiles.length + " " + qsTr("files")
                                        font.pointSize: 10
                                        color: Material.secondaryTextColor
                                    }
                                }

                                ColumnLayout {
                                    visible: root.isContentUpdating
                                    Layout.fillWidth: true
                                    spacing: 6

                                    Label {
                                        text: root.getUpdateStatusText(root.contentUpdateStatus)
                                        font.pointSize: 10
                                        color: Material.secondaryTextColor
                                    }

                                    ProgressBar {
                                        Layout.fillWidth: true
                                        from: 0
                                        to: 1
                                        value: root.contentUpdateProgress
                                        indeterminate: root.contentUpdateProgress <= 0
                                    }
                                }

                                Button {
                                    visible: root.installPath !== "" && !root.isContentUpdating
                                    text: qsTr("Update Workshop Content")
                                    icon.source: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_upload.svg"
                                    highlighted: true
                                    Layout.fillWidth: true
                                    onClicked: changeNoteDialog.open()
                                }

                                Label {
                                    visible: root.installPath === "" && root.itemFiles.length === 0
                                    text: qsTr("Item not installed locally. Subscribe to see and update the files.")
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                    color: Material.secondaryTextColor
                                    font.pointSize: 10
                                }

                                Repeater {
                                    model: root.itemFiles

                                    delegate: RowLayout {
                                        id: fileDelegate
                                        required property var modelData
                                        required property int index

                                        Layout.fillWidth: true
                                        spacing: 8

                                        Label {
                                            text: fileDelegate.modelData["name"]
                                            elide: Text.ElideMiddle
                                            Layout.fillWidth: true
                                            color: Material.foreground
                                            font.pointSize: 10
                                        }

                                        Label {
                                            text: root.formatFileSize(fileDelegate.modelData["size"])
                                            color: Material.secondaryTextColor
                                            font.pointSize: 10
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredHeight: 20
                }
            }
        }
    }

    Dialog {
        id: changeNoteDialog
        title: qsTr("Update Workshop Content")
        modal: true
        anchors.centerIn: parent
        width: 420
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            root.isContentUpdating = true
            root.contentUpdateProgress = 0
            root.steamWorkshop.itemOps.updateItemContent(root.publishedFileID, root.installPath, changeNoteField.text)
        }

        ColumnLayout {
            width: parent.width
            spacing: 12

            Label {
                text: qsTr("Modify the files in the installed folder, then submit your changes here.")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                color: Material.foreground
            }

            Label {
                text: qsTr("Change note (optional):")
                color: Material.foreground
            }

            TextArea {
                id: changeNoteField
                Layout.fillWidth: true
                Layout.minimumHeight: 80
                placeholderText: qsTr("Describe what changed...")
                wrapMode: Text.WordWrap
            }

            Label {
                text: qsTr("Content folder: %1").arg(root.installPath)
                color: Material.secondaryTextColor
                font.pointSize: 10
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }
    }

    Dialog {
        id: deleteConfirmDialog
        objectName: "deleteConfirmDialog"

        title: qsTr("⚠️ Delete Workshop Item")
        modal: true
        anchors.centerIn: parent
        width: 420

        ColumnLayout {
            width: parent.width
            spacing: 16

            RowLayout {
                spacing: 16
                Layout.fillWidth: true

                Label {
                    text: "⚠️"
                    font.pointSize: 36
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("This action cannot be undone!")
                        font.bold: true
                        font.pointSize: 12
                        color: Material.color(Material.Red)
                    }

                    Label {
                        text: qsTr("Are you sure you want to permanently delete:")
                        color: Material.foreground
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "\"" + root.itemTitle + "\""
                        font.italic: true
                        font.bold: true
                        color: Material.foreground
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Material.dividerColor
            }

            Label {
                text: qsTr("• All %1 subscribers will lose access\n• Statistics and comments will be removed\n• The workshop ID cannot be reused").arg(root.subscriptionCount)
                color: Material.secondaryTextColor
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                font.pointSize: 10
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Cancel")
                    enabled: !root.isDeleting
                    onClicked: deleteConfirmDialog.close()
                }

                Button {
                    text: qsTr("Delete Permanently")
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
                    icon.color: "white"
                    enabled: !root.isDeleting
                    onClicked: {
                        root.isDeleting = true
                        root.steamWorkshop.itemOps.deleteItem(root.publishedFileID)
                    }
                }

                BusyIndicator {
                    visible: root.isDeleting
                    running: root.isDeleting
                    implicitWidth: 28
                    implicitHeight: 28
                }
            }
        }
    }

    Dialog {
        id: errorDialog

        property int errorCode: 0

        function getErrorMessage(code: int): string {
            switch (code) {
            case 15:
                return qsTr("Access denied. Possible reasons:") + "<br><br>" + "• " + qsTr("This item may be under review. Check the status on the <a href='steam://url/CommunityFilePage/%1'>workshop page</a>").arg(root.publishedFileID) + "<br>" + "• " + qsTr("You do not have permission to edit this item") + "<br>" + "• " + qsTr("You need to <a href='https://steamcommunity.com/sharedfiles/workshoplegalagreement'>accept the Steam Workshop legal agreement</a>")
            case 9:
                return qsTr("Invalid parameter:") + "<br><br>" + "• " + qsTr("Check your title is not empty or too long") + "<br>" + "• " + qsTr("Check your description length") + "<br>" + "• " + qsTr("Check your tags are valid")
            case 25:
                return qsTr("Update limit exceeded:") + "<br><br>" + "• " + qsTr("Please wait a while before trying again")
            case 24:
                return qsTr("Workshop legal agreement has not been accepted:") + "<br><br>" + "• " + qsTr("Please <a href='https://steamcommunity.com/sharedfiles/workshoplegalagreement'>accept the agreement here</a>")
            default:
                return qsTr("Steam returned error code %1:").arg(code) + "<br><br>" + "• " + qsTr("Please try again later")
            }
        }

        title: qsTr("⚠️ Update Failed")
        modal: true
        anchors.centerIn: parent
        width: 420
        standardButtons: Dialog.Ok

        ColumnLayout {
            width: parent.width
            spacing: 12

            Label {
                text: errorDialog.getErrorMessage(errorDialog.errorCode)
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                color: Material.foreground
                font.pointSize: 11
                onLinkActivated: link => Qt.openUrlExternally(link)

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }

    component StatCard: SPCore.ImageBlurContainer {
        id: statCard

        required property string title
        required property string value
        required property string iconSource

        backgroundSource: backgroundImage
        flickable: scrollView
        stackView: root.stackView
        radius: 8
        implicitHeight: 70

        ColumnLayout {
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 2

            RowLayout {
                spacing: 6

                SPCore.ColorImage {
                    source: statCard.iconSource
                    sourceSize: Qt.size(14, 14)
                    opacity: 0.7
                }

                Label {
                    text: statCard.title
                    font.pointSize: 9
                    color: Material.secondaryTextColor
                }
            }

            Label {
                text: statCard.value
                font.pointSize: 14
                font.bold: true
                color: Material.foreground
            }
        }
    }
}
