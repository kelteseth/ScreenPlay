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
    property string editTitle: ""
    property string editDescription: ""
    property var editTags: []
    property int editVisibility: 0

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
        root.steamWorkshop.requestProfileItemDetails(root.publishedFileID)
    }

    function saveChanges(): void {
        root.isSaving = true
        if (root.editVisibility !== root.visibility) {
            root.steamWorkshop.updateItemVisibility(root.publishedFileID, root.editVisibility)
        }
        root.steamWorkshop.updateItemMetadata(
            root.publishedFileID,
            root.editTitle,
            root.editDescription,
            root.editTags
        )
    }

    function formatFileSize(bytes: int): string {
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1024 * 1024) return Math.round(bytes / 1024) + " KB"
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
            case 0: return qsTr("Public")
            case 1: return qsTr("Friends Only")
            case 2: return qsTr("Private")
            case 3: return qsTr("Unlisted")
            default: return qsTr("Unknown")
        }
    }

    Connections {
        target: root.steamWorkshop

        function onRequestProfileItemDetailReturned(
            publishedFileId: var,
            title: string,
            description: string,
            tags: var,
            steamIDOwner: int,
            votesUp: int,
            votesDown: int,
            score: real,
            url: string,
            fileSize: var,
            totalFileSize: var,
            previewUrl: string,
            timeCreated: int,
            timeUpdated: int,
            visibility: int,
            banned: bool,
            acceptedForUse: bool,
            subscriptionCount: int,
            favoriteCount: int,
            followerCount: int,
            uniqueWebsiteViews: int,
            numChildren: int
        ): void {
            if (publishedFileId !== root.publishedFileID) return

            root.itemTitle = title
            root.itemDescription = description
            root.itemTags = tags
            root.votesUp = votesUp
            root.votesDown = votesDown
            root.score = score
            root.itemUrl = url
            root.fileSize = fileSize
            root.totalFileSize = totalFileSize
            root.timeCreated = timeCreated
            root.timeUpdated = timeUpdated
            root.visibility = visibility
            root.banned = banned
            root.acceptedForUse = acceptedForUse
            root.subscriptionCount = subscriptionCount
            root.favoriteCount = favoriteCount
            root.followerCount = followerCount
            root.uniqueWebsiteViews = uniqueWebsiteViews
            root.numChildren = numChildren
            root.isLoading = false
        }

        function onWorkshopItemDeleted(success: bool, publishedFileID: var): void {
            if (publishedFileID === root.publishedFileID && success) {
                root.stackView.pop()
            }
        }

        function onWorkshopItemMetadataUpdated(success: bool, publishedFileID: var): void {
            if (publishedFileID !== root.publishedFileID) return
            root.isSaving = false
            if (success) {
                root.itemTitle = root.editTitle
                root.itemDescription = root.editDescription
                root.itemTags = root.editTags
                root.visibility = root.editVisibility
                root.state = "view"
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: root.isLoading
        visible: root.isLoading
    }

    Flickable {
        id: scrollView
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: mainColumn.implicitHeight + 40
        visible: !root.isLoading
        clip: true

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        ColumnLayout {
            id: mainColumn
            width: parent.width
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 350

                Image {
                    anchors.fill: parent
                    source: root.previewImageUrl
                    fillMode: Image.PreserveAspectCrop

                    Rectangle {
                        anchors.fill: parent
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "transparent" }
                            GradientStop { position: 0.4; color: "transparent" }
                            GradientStop { position: 0.85; color: Qt.rgba(Material.backgroundColor.r, Material.backgroundColor.g, Material.backgroundColor.b, 0.8) }
                            GradientStop { position: 1.0; color: Material.backgroundColor }
                        }
                    }
                }

                Button {
                    anchors {
                        top: parent.top
                        left: parent.left
                        margins: 20
                    }
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_left.svg"
                    icon.color: "white"
                    flat: true
                    onClicked: root.stackView.pop()
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Back to Profile")
                }

                RowLayout {
                    anchors {
                        top: parent.top
                        right: parent.right
                        margins: 20
                    }
                    spacing: 10

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
                }

                ColumnLayout {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                        margins: 20
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
                Layout.margins: 20
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

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        implicitHeight: detailsColumn.implicitHeight + 30
                        color: Material.dialogColor
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

                                Label { text: qsTr("File ID:"); color: Material.secondaryTextColor }
                                Label { text: root.publishedFileID; color: Material.foreground; Layout.fillWidth: true }

                                Label { text: qsTr("File Size:"); color: Material.secondaryTextColor }
                                Label { text: root.formatFileSize(root.totalFileSize > 0 ? root.totalFileSize : root.fileSize); color: Material.foreground }

                                Label { text: qsTr("Created:"); color: Material.secondaryTextColor }
                                Label { text: root.formatDateTime(root.timeCreated); color: Material.foreground }

                                Label { text: qsTr("Updated:"); color: Material.secondaryTextColor }
                                Label { text: root.formatDateTime(root.timeUpdated); color: Material.foreground }

                                Label { text: qsTr("Visibility:"); color: Material.secondaryTextColor }
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

                                Label { text: qsTr("Followers:"); color: Material.secondaryTextColor; visible: root.followerCount > 0 }
                                Label { text: root.followerCount.toLocaleString(); color: Material.foreground; visible: root.followerCount > 0 }
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        spacing: 16

                        Rectangle {
                            Layout.fillWidth: true
                            implicitHeight: actionsColumn.implicitHeight + 30
                            color: Material.dialogColor
                            radius: 8

                            ColumnLayout {
                                id: actionsColumn
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    margins: 15
                                }
                                spacing: 12

                            Label {
                                text: qsTr("Actions")
                                font.pointSize: 14
                                font.bold: true
                                color: Material.foreground
                            }

                            Flow {
                                Layout.fillWidth: true
                                spacing: 10

                                Button {
                                    visible: !root.isEditMode
                                    text: qsTr("Edit Item")
                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_edit.svg"
                                    icon.color: "white"
                                    highlighted: true
                                    onClicked: root.state = "edit"
                                }

                                Button {
                                    visible: root.isEditMode
                                    text: qsTr("Save Changes")
                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_done.svg"
                                    icon.color: "white"
                                    highlighted: true
                                    enabled: !root.isSaving
                                    onClicked: root.saveChanges()
                                }

                                Button {
                                    visible: root.isEditMode
                                    text: qsTr("Cancel")
                                    flat: true
                                    onClicked: root.state = "view"
                                }

                                BusyIndicator {
                                    visible: root.isSaving
                                    running: root.isSaving
                                    implicitWidth: 24
                                    implicitHeight: 24
                                }

                                Button {
                                    text: qsTr("Open in Steam")
                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_steam.svg"
                                    icon.color: "transparent"
                                    flat: true
                                    onClicked: Qt.openUrlExternally("steam://url/CommunityFilePage/" + root.publishedFileID)
                                }

                                Button {
                                    text: qsTr("Workshop Page")
                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
                                    icon.color: Material.iconColor
                                    flat: true
                                    onClicked: Qt.openUrlExternally("https://steamcommunity.com/sharedfiles/filedetails/?id=" + root.publishedFileID)
                                }

                                Button {
                                    text: qsTr("Delete Item")
                                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_delete.svg"
                                    icon.color: "white"
                                    Material.background: Material.Red
                                    onClicked: deleteConfirmDialog.open()
                                }
                            }

                            Label {
                                visible: root.isEditMode
                                text: qsTr("Changes are uploaded directly to Steam Workshop.")
                                color: Material.secondaryTextColor
                                font.pointSize: 10
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }

                            Label {
                                visible: !root.isEditMode
                                text: qsTr("Click 'Edit Item' to modify title, description, tags or visibility.")
                                color: Material.secondaryTextColor
                                font.pointSize: 10
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                    }

                        Rectangle {
                            Layout.fillWidth: true
                            implicitHeight: tagsColumn.implicitHeight + 30
                            color: Material.dialogColor
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
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: descColumn.implicitHeight + 30
                    color: Material.dialogColor
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

                Item { Layout.preferredHeight: 20 }
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

                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("Cancel")
                    flat: true
                    onClicked: deleteConfirmDialog.close()
                }

                Button {
                    text: qsTr("Delete Permanently")
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg"
                    icon.color: "white"
                    Material.background: Material.Red
                    onClicked: {
                        root.steamWorkshop.deleteItem(root.publishedFileID)
                        deleteConfirmDialog.close()
                    }
                }
            }
        }
    }

    component StatCard: Rectangle {
        id: statCard

        required property string title
        required property string value
        required property string iconSource

        color: Material.dialogColor
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

                Image {
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
