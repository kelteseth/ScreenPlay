import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12
import QtWebEngine 1.8

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: root

    property var codec: Create.VP8

    signal next

    Timer {
        running: true
        interval: 1000
        onTriggered: webView.url = "https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance"
    }

    WebEngineView {
        id: webView
        backgroundColor: "gray"
        width: parent.width * .66
        anchors {
            margins: 20
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
    }

    Column {
        spacing: 10

        anchors {
            top: parent.top
            left: webView.right
            bottom: parent.bottom
            right: parent.right
            margins: 40
        }

        Headline {
            Layout.alignment: Qt.AlignTop
            text.text: qsTr("Import a video")
        }

        Item {
            width: parent.width
            height: 5
        }

        Text {
            id: txtDescription
            text: qsTr("Depending on your PC configuration it is better to convert your wallpaper to a specific video codec. If both have bad performance you can also try a QML wallpaper!")
            color: "gray"
            width: parent.width
            font.pointSize: 13
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        Item {
            width: parent.width
            height: 50
        }

        Text {
            id: txtComboboxHeadline
            text: qsTr("Set your preffered video codec:")
            color: "gray"
            width: parent.width
            font.pointSize: 14
        }
        ComboBox {
            id: comboBoxCodec
            textRole: "text"
            width: 300
            valueRole: "value"
            onCurrentIndexChanged: {
                root.codec = model.get(comboBoxCodec.currentIndex).value
            }

            model: ListModel {
                id: model
                ListElement {
                    text: "VP8 (Old Intel best)"
                    value: Create.VP8
                }
                ListElement {
                    text: "VP9 (AMD/Nvidia best)"
                    value: Create.VP9
                }
            }
        }

    }

    Button {
        text: qsTr("Open Documentation")
        Material.background: Material.LightGreen
        Material.foreground: "white"
        icon.source: "qrc:/assets/icons/icon_document.svg"
        icon.color: "white"
        icon.width: 16
        icon.height: 16
        onClicked: Qt.openUrlExternally(
                       "https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance")
        anchors {
            left: webView.right
            bottom: parent.bottom
            margins: 20
        }
    }
    Button {
        text: qsTr("Next")
        highlighted: true
        onClicked: {
            root.next()
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
    }
}
