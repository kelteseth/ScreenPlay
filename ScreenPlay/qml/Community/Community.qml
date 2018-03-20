import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.XmlListModel 2.0

Item {
    id: community
    anchors.fill: parent

    Rectangle {
        id: stomtWrapper
        width: 500
        color: "gray"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        LinearGradient {
            id: tabShadow
            antialiasing: true
            cached: true
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(stomtWrapper.width, stomtWrapper.height)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#199EF1"
                }
                GradientStop {
                    position: 1.0
                    color: "#092E6C"
                }
            }
        }
        Image {
            id: imgStomt
            source: "qrc:/assets/images/Stomt_Logo+Wordmark_Mono_BLK.svg"
            width: 250
            height: 250
            sourceSize: Qt.size(250, 250)
            anchors {
                top: parent.top
                topMargin: -30
                horizontalCenter: parent.horizontalCenter
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: Qt.openUrlExternally("https://stomt.com/")
            }
        }
        Text {
            id:txtStomtDescription
            font.pointSize: 14
            color: "white"
            height:100
            text: qsTr("We use Stomt because it provides quick and easy feedback via I like/I wish. So you can easily give us feedback and speak your mind. We will read these wishes on a daily basis!")
            font.family: "Roboto"
            font.weight: Font.Normal
            wrapMode: Text.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            renderType: Text.NativeRendering
            anchors {
                top: imgStomt.bottom
                topMargin: -50
                right: parent.right
                rightMargin: 50
                left: parent.left
                leftMargin: 50
            }
        }

        Feedback {
            id: feedback
            height: 236
            width: 420
            anchors {
                top: txtStomtDescription.bottom
                topMargin: 0
                horizontalCenter: parent.horizontalCenter
            }
        }

        Button {
            id: btnStomt
            text: qsTr("Open ScreenPlay Stomt page")
            anchors {
                bottom: parent.bottom
                bottomMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
            Material.background: Material.Orange
            Material.foreground: "white"

            icon.source: "qrc:/assets/icons/icon_share.svg"
            icon.color: "white"
            icon.width: 16
            icon.height: 16
            onClicked: {
                Qt.openUrlExternally("https://www.stomt.com/screenplay")
            }
        }
    }

    LinearGradient {
        id: tabShadowFoot
        height: 4
        z: 500
        cached: true
        anchors {
            bottom: footer.top
            right: parent.right
            left: stomtWrapper.right
        }
        start: Qt.point(0, 4)
        end: Qt.point(0, 0)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#33000000"
            }
            GradientStop {
                position: 1.0
                color: "#00000000"
            }
        }
    }

    XmlListModel {
        id: feedModel
        source: "http://screen-play.rocks/index.php?option=com_content&view=category&layout=blog&id=10&format=feed&type=rss"
        query: "/rss/channel/item"
        XmlRole {
            name: "title"
            query: "title/string()"
        }
        XmlRole {
            name: "backgroundImage"
            query: "description/string()"
        }
        XmlRole {
            name: "link"
            query: "link/string()"
        }
        XmlRole {
            name: "pubDate"
            query: "pubDate/string()"
        }
        XmlRole {
            name: "category"
            query: "category/string()"
        }
    }

    GridView {
        id: changelogFlickableWrapper
        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 5000
        flickDeceleration: 5000
        cellHeight: 205
        cellWidth: 360
        model: feedModel

        anchors {
            top: parent.top
            topMargin: 20
            right: parent.right
            rightMargin: 20
            bottom: footer.top

            left: stomtWrapper.right
            leftMargin: 90
        }

        header: Item {
            height: 100
            width: parent.width

            Text {
                id: name
                text: qsTr("News & Patchnotes")
                wrapMode: Text.WordWrap
                color: "#626262"
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 32
                font.family: "Roboto"

                anchors {
                    top: parent.top
                    topMargin: 30
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

        delegate: Item {
            id: root
            width: 352
            height: 197

            RectangularGlow {
                id: effectchangelogWrapper
                anchors {
                    top: parent.top
                    topMargin: 3
                }
                height: parent.height
                width: parent.width
                cached: true
                glowRadius: 3
                spread: 0.2
                color: "black"
                opacity: 0.2
                cornerRadius: 15
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 5
                radius: 4

                Image {
                    anchors.fill: parent
                    source: {
                        var a = backgroundImage
                        var r = new RegExp(/<img[^>]+src="([^">]+)"/)
                        var url = r.exec(a)
                        return url[1].toString()
                    }
                }

                LinearGradient {
                    visible: true
                    opacity: .5
                    anchors.fill: parent
                    start: Qt.point(0, parent.height)
                    end: Qt.point(0, parent.height - 150)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#BB000000"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#00000000"
                        }
                    }
                }

                Text {
                    id: txtTitle
                    text: title
                    renderType: Text.NativeRendering
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        left: parent.left
                        margins: 20
                    }
                    color: "white"
                    font.family: "Roboto"
                    font.weight: Font.Normal
                    font.pixelSize: 18
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: txtPubDate
                    text: {
                        return pubDate.replace("+0000", "")
                    }
                    anchors {
                        right: parent.right
                        rightMargin: 20
                        bottom: txtTitle.top
                        bottomMargin: 10
                        left: parent.left
                        leftMargin: 20
                    }
                    color: "white"
                    font.family: "Roboto"
                    renderType: Text.NativeRendering
                    font.weight: Font.Normal
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.openUrlExternally(link)
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
            policy: ScrollBar.AlwaysOn
        }
    }

    Rectangle {
        id: footer
        height: 80
        z: 100
        anchors {
            right: parent.right
            bottom: parent.bottom
            left: stomtWrapper.right
        }
        Row {
            anchors.centerIn: parent
            spacing: 20
            Button {
                text: qsTr("Forums")
                Material.background: Material.Blue
                Material.foreground: "white"
                onClicked: Qt.openUrlExternally("http://forum.screen-play.rocks:4567/")
                icon.source: "qrc:/assets/icons/icon_people.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
            }
            Button {
                text: qsTr("Documentation")
                Material.background: Material.LightGreen
                Material.foreground: "white"
                icon.source: "qrc:/assets/icons/icon_document.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally("https://qmlbook.github.io/index.html")
            }
            Button {
                text: qsTr("Github")
                Material.background: Material.Orange
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_code.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally("https://github.com/Aimber/")
            }
            Button {
                text: qsTr("Workshop")
                Material.background: Material.Red
                Material.foreground: "white"

                icon.source: "qrc:/assets/icons/icon_steam.svg"
                icon.color: "white"
                icon.width: 16
                icon.height: 16
                onClicked: Qt.openUrlExternally(
                               "steam://url/SteamWorkshopPage/672870")
            }
        }
    }
}
