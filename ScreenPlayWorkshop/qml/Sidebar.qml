import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ScreenPlayWorkshop
import ScreenPlayCore as SPCore

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
    property bool subscriptionStateKnown: false
    property var creatorSteamID: null
    property string creatorName: ""

    signal tagClicked(var tag)
    signal unsubscribed(var publishedFileID)
    signal creatorSearchRequested(string creatorName, var creatorSteamID)

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
        root.subscriptionStateKnown = false
        root.creatorName = ""
        root.creatorSteamID = null
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
    topPadding: 0

    onClosed: {
        root.creatorName = ""
        root.creatorSteamID = null
    }

    Connections {
        function onRequestItemDetailReturned(detail) {
            root.subscribed = steamWorkshop.isSubscribed(detail.publishedFileId)
            root.subscriptionStateKnown = true
            root.creatorSteamID = detail.steamIDOwner
            tagListModel.clear();
            // Even if the tags array is empty it still contains
            // one empty string, resulting in an empty button
            if (detail.tags.length > 1) {
                for (var i in detail.tags) {
                    tagListModel.append({
                        "name": detail.tags[i]
                    })
                }
                rpTagList.model = tagListModel
            } else {
                rpTagList.model = null
            }
            txtTitle.text = detail.title
            const size = Math.floor((1000 * ((detail.fileSize / 1024) / 1000)) / 1000)
            txtFileSize.text = qsTr("Size: ") + size + " MB"
            root.votesUp = detail.votesUp
            root.votesDown = detail.votesDown
            let desc = detail.description
            if (desc === "")
                desc = qsTr("No description...")
            txtDescription.text = desc
        }

        function onCreatorNameReady(name: string, steamID64: string): void {
            // Sidebar tracks one item at a time; use the precise C++ string to override the quint64 var
            root.creatorName = name
            root.creatorSteamID = steamID64
        }

        target: steamWorkshop
    }

    Item {
        id: imgWrapper
        // Keys must be used in an Item and Drawer is not an Item...
        Keys.onEscapePressed: root.close()
        focus: true
        height: 220

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

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

            SPCore.ColorImage {
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
                    ToolTip.visible: voteHover.hovered
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

                    Text {
                        visible: rpTagList.count === 0
                        width: tagsFlickable.width
                        height: parent.height
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: qsTr("No tags o((>ω< ))o")
                        color: Material.secondaryTextColor
                        font.pointSize: 10
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
        }

        ToolButton {
            id: btnCreator

            font.pointSize: 10
            icon.source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_search.svg"
            height: 25
            visible: root.creatorName !== ""
            text: root.creatorName ? qsTr("More by %1").arg(root.creatorName) : ""
            onClicked: {
                root.close()
                root.creatorSearchRequested(root.creatorName, root.creatorSteamID)
            }

            Behavior on implicitWidth {
                SmoothedAnimation {
                    velocity: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Button {
            id: btnSubscribe

            enabled: root.subscriptionStateKnown
            highlighted: true
            Material.accent: root.subscribed ? Material.color(Material.Red) : root.Material.accent
            icon.source: !root.subscriptionStateKnown ? "" : root.subscribed ? "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_close.svg" : "qrc:/qt/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
            text: !root.subscriptionStateKnown ? qsTr("Loading...") : root.subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
            ToolTip.visible: hovered && root.subscribed
            ToolTip.delay: 500
            ToolTip.text: qsTr("Steam will delete the content from your PC once ScreenPlay no longer runs.")

            Behavior on implicitWidth {
                SmoothedAnimation {
                    velocity: 200
                    easing.type: Easing.InOutQuad
                }
            }

            onClicked: {
                if (root.subscribed) {
                    root.subscribed = false
                    root.steamWorkshop.unsubscribeItem(root.publishedFileID)
                    root.unsubscribed(root.publishedFileID)
                    root.close()
                } else {
                    root.subscribed = true
                    root.steamWorkshop.subscribeItem(root.publishedFileID)
                    root.close()
                }
            }
        }
    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Qt.darker(Material.background)
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
