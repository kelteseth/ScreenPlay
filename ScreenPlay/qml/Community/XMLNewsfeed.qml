import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.XmlListModel 2.15
import ScreenPlay 1.0

GridView {
    id: changelogFlickableWrapper
    flickableDirection: Flickable.VerticalFlick
    maximumFlickVelocity: 5000
    flickDeceleration: 5000
    cellHeight: 205
    cellWidth: 360
    clip: true
    model: feedModel
    onCountChanged: print("count ",count)


    XmlListModel {
        id: feedModel
        namespaceDeclarations: "declare default element namespace 'http://www.w3.org/2005/Atom';"
        onProgressChanged: print("progress ",progress)
        source: "https://gitlab.com/kelteseth/ScreenPlay/-/tags?&format=atom"
        query: "/entry"
        XmlRole {
            name: "title"
            query: "title/string()"
        }
        XmlRole {
            name: "content"
            query: "content/string()"
        }
        XmlRole {
            name: "link"
            query: "link/string()"
        }
        XmlRole {
            name: "updated"
            query: "updated/string()"
        }
    }

    header: Item {
        height: 100
        width: parent.width

        Text {
            id: name
            text: qsTr("News & Patchnotes")
            wrapMode: Text.WordWrap
            color: "#626262"
            
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pointSize: 32
            font.family: ScreenPlay.settings.font

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

            Text {
                id: txtTitle
                text: title
                
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.left
                    margins: 20
                }
                color: "#333333"
                font.family: ScreenPlay.settings.font
                font.weight: Font.Normal
                font.pointSize: 18
                wrapMode: Text.WordWrap
            }
            Text {
                id: txtPubDate
                text: updated
                anchors {
                    right: parent.right
                    rightMargin: 20
                    bottom: txtTitle.top
                    bottomMargin: 10
                    left: parent.left
                    leftMargin: 20
                }
                color: "white"
                font.family: ScreenPlay.settings.font
                
                font.weight: Font.Normal
                font.pointSize: 14
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
