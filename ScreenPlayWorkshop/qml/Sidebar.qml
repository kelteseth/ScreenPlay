import QtQuick
import Qt5Compat.GraphicalEffects
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
            tagListModel.clear()
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
            pbVotes.to = votesDown + votesUp
            pbVotes.value = votesUp
            txtVotesDown.text = votesDown
            txtVotesUp.text = votesUp
            if (description === "")
                description = qsTr("No description...")

            txtDescription.text = description
            pbVotes.hoverText = votesUp + " / " + votesDown
        }

        target: steamWorkshop
    }

    Item {
        id: imgWrapper

        width: parent.width
        height: 220

        Image {
            id: img

            fillMode: Image.PreserveAspectCrop
            anchors.fill: parent
        }



        LinearGradient {
            height: 50
            cached: true
            start: Qt.point(0, 50)
            end: Qt.point(0, 0)

            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
            }

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#EE000000"
                }

                GradientStop {
                    position: 1
                    color: "#00000000"
                }
            }
        }

        Text {
            id: txtTitle
            font.weight: Font.Thin
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

                source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_arrow_right.svg"
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

            ColumnLayout {
                Layout.maximumWidth: 280
                Layout.alignment: Qt.AlignHCenter

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    ToolButton {
                        id: txtVotesUp

                        Layout.fillWidth: true
                        icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_up.svg"
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

                        Layout.fillWidth: true
                        icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_thumb_down.svg"
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Click here if you do not like the content")
                        onClicked: {
                            steamWorkshop.vote(root.publishedFileID, false)
                            txtVotesUp.highlighted = false
                            txtVotesDown.highlighted = true
                        }
                    }
                }

                ProgressBar {
                    id: pbVotes

                    property string hoverText

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    ToolTip.visible: hovered
                    ToolTip.text: hoverText
                }
            }

            Flickable {
                Layout.preferredHeight: 40
                Layout.maximumHeight: 40
                Layout.fillWidth: true
                clip: true
                contentWidth: rowTagList.width + rpTagList.count * rowTagList.spacing

                ListModel {
                    id: tagListModel
                }

                Row {
                    id: rowTagList

                    width: parent.width
                    spacing: 10

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
                Layout.minimumHeight: 150
                Layout.fillHeight: true //txtDescription.paintedHeight > 100
                color: Material.backgroundColor
                radius: 3

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 20
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    Text {
                        id: txtDescription

                        width: parent.width
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
            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_open_in_new.svg"
            height: 25
            text: qsTr("Open In Steam")
            onClicked: Qt.openUrlExternally(
                           "steam://url/CommunityFilePage/" + root.publishedFileID)
        }

        Button {
            id: btnSubscribe

            highlighted: !root.subscribed
            enabled: !root.subscribed
            icon.source: "qrc:/qml/ScreenPlayWorkshop/assets/icons/icon_download.svg"
            text: root.subscribed ? qsTr("Subscribed!") : qsTr("Subscribe")
            onClicked: {
                root.subscribed = true
                root.steamWorkshop.subscribeItem(root.publishedFileID)
            }
        }
    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Qt.darker(
                                                       Material.background)
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
