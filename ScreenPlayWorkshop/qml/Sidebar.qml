import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlayWorkshop

Drawer {
    id: root

    property SteamWorkshop steamWorkshop
    property url videoPreview
    property alias imgUrl: img.source
    property string name
    property var publishedFileID
    property int itemIndex
    property int subscriptionCount
    property int votesUp: 0
    property int votesDown: 0
    property bool subscribed: false

    signal tagClicked(var tag)

    function setWorkshopItem(publishedFileID, imgUrl, videoPreview, subscriptionCount) {
        if (root.publishedFileID === publishedFileID) {
            if (!root.visible)
                root.open()
            else
                root.close()
            return
        }
        root.publishedFileID = publishedFileID
        root.imgUrl = imgUrl
        root.subscriptionCount = subscriptionCount
        root.videoPreview = videoPreview
        root.subscribed = false
        txtVotesUp.highlighted = false
        txtVotesDown.highlighted = false
        if (!root.visible)
            root.open()
        steamWorkshop.requestWorkshopItemDetails(publishedFileID)
    }

    edge: Qt.RightEdge
    height: parent.height - 60
    dim: false
    modal: false
    width: 400
    interactive: false

    Connections {
        function onRequestItemDetailReturned(title, tags, steamIDOwner, description, votesUp, votesDown, url, fileSize, publishedFileId) {
            root.subscribed = steamWorkshop.isSubscribed(publishedFileId)
            tagListModel.clear();
            // Even if the tags array is empty it still contains
            // one empty string, resulting in an empty button
            if (tags.length > 1) {
                for (var i in tags) {
                    tagListModel.append({
                        "name": tags[i]
                    })
                }
                rpTagList.model = tagListModel
            } else {
                rpTagList.model = null
            }
            txtTitle.text = title
            const size = Math.floor((1000 * ((fileSize / 1024) / 1000)) / 1000)
            txtFileSize.text = qsTr("Size: ") + size + " MB"
            root.votesUp = votesUp
            root.votesDown = votesDown
            if (description === "")
                description = qsTr("No description...")
            txtDescription.text = description
        }

        target: steamWorkshop
    }

    Item {
        id: imgWrapper
        // Keys must be used in an Item and Drawer is not an Item...
        Keys.onEscapePressed: root.close()
        focus: true
        width: parent.width
        height: 220

        Image {
            id: img

            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
        }

        Rectangle {
            height: 50

            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
            }

            gradient: Gradient {
                GradientStop {
                    position: 1
                    color: "#EE000000"
                }

                GradientStop {
                    position: 0
                    color: "#00000000"
                }
            }
        }

        Text {
            id: txtTitle
            verticalAlignment: Text.AlignBottom
            font.pointSize: 16
            color: "white"
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            height: 50

            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: 20
                left: parent.left
            }
        }

        MouseArea {
            id: button

            height: 50
            width: 50
            anchors.top: parent.top
            anchors.left: parent.left
            cursorShape: Qt.PointingHandCursor
            onClicked: root.close()

            Image {
                id: imgBack

                source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_right.svg"
                sourceSize: Qt.size(15, 15)
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
            }
        }
    }

    ColumnLayout {
        spacing: 20

        anchors {
            top: imgWrapper.bottom
            right: parent.right
            left: parent.left
            bottom: rlBottomButtons.top
            margins: 20
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 20

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                spacing: 10

                Text {
                    id: txtVotePercentage

                    property int total: root.votesUp + root.votesDown

                    Layout.fillWidth: true
                    color: Material.primaryTextColor
                    font.pointSize: 14
                    font.bold: true
                    text: total > 0 ? Math.round((root.votesUp / total) * 100) + qsTr("% positive") : qsTr("No votes yet")
                    ToolTip.visible: hovered
                    ToolTip.text: root.votesUp + " 👍  /  " + root.votesDown + " 👎"

                    HoverHandler {
                        id: voteHover
                    }
                }

                ToolButton {
                    id: txtVotesUp

                    text: root.votesUp
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_up.svg"
                    icon.color: Material.iconColor
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Click here if you like the content")
                    onClicked: {
                        steamWorkshop.vote(root.publishedFileID, true)
                        txtVotesUp.highlighted = true
                        txtVotesDown.highlighted = false
                    }
                }

                ToolButton {
                    id: txtVotesDown

                    text: root.votesDown
                    icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_down.svg"
                    icon.color: Material.iconColor
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Click here if you do not like the content")
                    onClicked: {
                        steamWorkshop.vote(root.publishedFileID, false)
                        txtVotesUp.highlighted = false
                        txtVotesDown.highlighted = true
                    }
                }
            }

            Flickable {
                id: tagsFlickable
                Layout.alignment: Qt.AlignTop
                Layout.preferredHeight: 55
                Layout.maximumHeight: 55
                Layout.fillWidth: true
                clip: true
                flickableDirection: Flickable.HorizontalFlick
                ScrollBar.horizontal: ScrollBar {
                    height: 5
                }

                WheelHandler {
                    orientation: Qt.Vertical
                    onWheel: event => {
                        tagsFlickable.contentX = Math.max(0, Math.min(tagsFlickable.contentX - event.angleDelta.y, tagsFlickable.contentWidth - tagsFlickable.width))
                    }
                }
                contentWidth: rpTagList.childrenRect.width + rowTagList.width + (rpTagList.count * rowTagList.spacing)
                contentHeight: 40

                Row {
                    id: rowTagList
                    height: parent.height
                    spacing: 10

                    ListModel {
                        id: tagListModel
                    }

                    Repeater {
                        id: rpTagList

                        delegate: Button {
                            id: txtTags

                            property string tags

                            text: name
                            font.pointSize: 8
                            onClicked: root.tagClicked(txtTags.text)
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                spacing: 20

                Text {
                    id: txtSubscriptionCount

                    color: Material.secondaryTextColor
                    font.pointSize: 11
                    text: qsTr("Subscribtions: ") + root.subscriptionCount
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }

                Item {
                    Layout.fillWidth: true
                }

                Text {
                    id: txtFileSize

                    color: Material.secondaryTextColor
                    font.pointSize: 11
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 150
                color: Material.backgroundColor
                radius: 3
                clip: true

                ScrollView {
                    id: descriptionScrollView

                    anchors.fill: parent
                    anchors.margins: 20
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    Text {
                        id: txtDescription

                        width: descriptionScrollView.availableWidth
                        color: Material.primaryTextColor
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }

    RowLayout {
        id: rlBottomButtons

        spacing: 20

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 20
        }

        ToolButton {
            id: btnOpenInSteam

            font.pointSize: 10
            icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
            icon.color: "transparent"
            height: 25
            text: qsTr("Open In Steam")
            onClicked: Qt.openUrlExternally("steam://url/CommunityFilePage/" + root.publishedFileID)
        }

        Button {
            id: btnSubscribe

            highlighted: true
            Material.accent: root.subscribed ? Material.Red : Material.primary
            icon.source: root.subscribed ? "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg" : "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
            text: root.subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
            onClicked: {
                if (root.subscribed) {
                    root.subscribed = false
                    root.steamWorkshop.unsubscribeItem(root.publishedFileID)
                } else {
                    root.subscribed = true
                    root.steamWorkshop.subscribeItem(root.publishedFileID)
                }
            }
        }
    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Qt.darker(Material.background)
        opacity: 0.95
    }

    enter: Transition {
        SmoothedAnimation {
            velocity: 10
            easing.type: Easing.InOutQuart
        }
    }

    exit: Transition {
        SmoothedAnimation {
            velocity: 10
            easing.type: Easing.InOutQuart
        }
    }
}
