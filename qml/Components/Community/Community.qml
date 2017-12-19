import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.XmlListModel 2.0
import QtQuick.Controls.Material 2.2

Item {
    id: community
    anchors.fill: parent


    //    XmlListModel {
    //        id: feedModel

    //        source: "https://rss.golem.de/rss.php?feed=RSS2.0"
    //        query: "/rss/channel/item"
    //        XmlRole { name: "title"; query: "title/string()" }

    //    }
    Item {
        id: communityWrapper
        width: 800
        height: 600

        anchors {
            top: parent.top
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        Button {
            text: qsTr("Open ScreenPlay Community")
            anchors.centerIn: parent
            Material.background: Material.Orange
            Material.foreground: "white"
            onClicked: {
                Qt.openUrlExternally("https://www.screen-play.rocks/")
            }
        }

        //        ListView {
        //            model: feedModel

        //            anchors {
        //                top:parent.top
        //                margins: 20
        //                right:parent.right
        //                left:parent.left
        //                bottom: parent.bottom
        //            }

        //            delegate: Text {
        //                text: title
        //            }
        //        }
    }
}
