import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.11
import QtWebEngine 1.8
import QtQuick.Controls.Material 2.2

import ScreenPlay.Workshop 1.0
import ScreenPlay 1.0

Drawer {
    id: root
    edge: Qt.RightEdge
    height: parent.height - 60
    dim: false
    modal: false
    width: 400
    interactive: false
    property SteamWorkshop steamWorkshop

    signal tagClicked(var tag)

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Qt.darker(
                                                       Material.background)
        opacity: .95
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

    Component.onCompleted: {
        WebEngine.settings.localContentCanAccessFileUrls = true
        WebEngine.settings.localContentCanAccessRemoteUrls = true
        WebEngine.settings.allowRunningInsecureContent = true
        WebEngine.settings.accelerated2dCanvasEnabled = true
        WebEngine.settings.javascriptCanOpenWindows = false
        WebEngine.settings.showScrollBars = false
        WebEngine.settings.playbackRequiresUserGesture = false
        WebEngine.settings.focusOnNavigationEnabled = true
    }

    property url videoPreview
    property alias imgUrl: img.source
    property string name
    property var publishedFileID
    property int itemIndex
    property int subscriptionCount
    property bool subscribed: false

    function setWorkshopItem(publishedFileID, imgUrl, videoPreview, subscriptionCount) {

        if (root.publishedFileID === publishedFileID) {
            if (!root.visible) {
                root.open()
            } else {
                root.close()
            }
            return
        }
        webView.opacity = 0
        root.publishedFileID = publishedFileID
        root.imgUrl = imgUrl
        root.subscriptionCount = subscriptionCount
        root.videoPreview = videoPreview
        root.subscribed = false
        txtVotesUp.highlighted = false
        txtVotesDown.highlighted = false

        if (!root.visible) {
            root.open()
        }

        steamWorkshop.requestWorkshopItemDetails(publishedFileID)

        webView.setVideo()
    }

    Connections {
        target: steamWorkshop
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
            txtFileSize.text = qsTr("Size: ") + size + qsTr(" MB")
            pbVotes.to = votesDown + votesUp
            pbVotes.value = votesUp
            txtVotesDown.text = votesDown
            txtVotesUp.text = votesUp
            if (description === "") {
                description = qsTr("No description...")
            }

            txtDescription.text = description
            pbVotes.hoverText = votesUp + " / " + votesDown
        }
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
        WebEngineView {
            id: webView
            anchors.fill: parent

            opacity: 0
            property bool ready: false
            url: "qrc:/assets/WorkshopPreview.html"
            onUrlChanged: print(url)

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                }
            }

            function getUpdateVideoCommand() {
                let src = ""
                src += "var video = document.getElementById('video');\n"
                src += "video.src = '" + root.videoPreview + "';\n"
                // Incase a workshop item has no gif preview
                src += "video.poster = '" + root.videoPreview + "';\n"
                src += "video.play();\n"

                return src
            }

            function setVideo() {
                if (!root.videoPreview.toString().startsWith("https"))
                    return

                webView.runJavaScript(getUpdateVideoCommand(),
                                      function (result) {
                                          webView.opacity = 1
                                      })
            }
        }

        LinearGradient {
            height: 50
            cached: true

            anchors {
                bottom: parent.bottom
                right: parent.right
                left: parent.left
            }
            start: Qt.point(0, 50)
            end: Qt.point(0, 0)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#EE000000"
                }
                GradientStop {
                    position: 1.0
                    color: "#00000000"
                }
            }
        }

        Text {
            id: txtTitle
            font.family: ScreenPlay.settings.font
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
                source: "qrc:/assets/icons/icon_arrow_right.svg"
                sourceSize: Qt.size(15, 15)
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
            }
        }
    }

    ColumnLayout {
        anchors {
            top: imgWrapper.bottom
            right: parent.right
            left: parent.left
            bottom: rlBottomButtons.top
            margins: 20
        }

        spacing: 20

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
                        icon.source: "qrc:/assets/icons/icon_thumb_up.svg"
                        font.family: ScreenPlay.settings.font
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
                        icon.source: "qrc:/assets/icons/icon_thumb_down.svg"
                        font.family: ScreenPlay.settings.font
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
                            font.family: ScreenPlay.settings.font
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
                    font.family: ScreenPlay.settings.font
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
                    font.family: ScreenPlay.settings.font
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
                        font.family: ScreenPlay.settings.font
                        font.pointSize: 12
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
            }
        }
    }
    RowLayout {
        id: rlBottomButtons
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 20
        }
        spacing: 20
        ToolButton {
            id: btnOpenInSteam
            font.pointSize: 10
            icon.source: "qrc:/assets/icons/icon_open_in_new.svg"
            height: 25
            text: qsTr("Open In Steam")
            onClicked: Qt.openUrlExternally(
                           "steam://url/CommunityFilePage/" + root.publishedFileID)
        }
        Button {
            id: btnSubscribe

            highlighted: !root.subscribed
            enabled: !root.subscribed
            icon.source: "qrc:/assets/icons/icon_download.svg"
            text: root.subscribed ? qsTr("Subscribed!") : qsTr("Subscribe")
            onClicked: {
                root.subscribed = true
                root.steamWorkshop.subscribeItem(root.publishedFileID)
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.75;height:800;width:300}
}
##^##*/

