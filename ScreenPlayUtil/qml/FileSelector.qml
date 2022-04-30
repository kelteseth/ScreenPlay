import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Dialogs
import ScreenPlayApp 1.0
import ScreenPlay 1.0

/*!
   \qmltype Image Selector
   \brief A image selector with popup preview.

   Test

   \image rootPreview.png

   \section1 Setting default text and capitalization

   Test

   \qml
        root {

        }
   \endqml

*/
Item {
    id: root

    property string file
    property alias placeHolderText: txtPlaceholder.text
    property alias fileDialog: fileDialog

    height: 65
    implicitWidth: 300
    state: "nothingSelected"
    onFileChanged: {
        if (file === "") {
            txtName.text = "";
            root.state = "nothingSelected";
        } else {
            root.state = "imageSelected";
        }
    }

    Rectangle {
        id: rectangle

        color: Material.theme === Material.Light ? Material.background : Qt.darker(Material.background)
        radius: 3
        clip: true

        anchors {
            fill: parent
            margins: 3
        }

        Text {
            id: txtPlaceholder

            clip: true
            font.pointSize: 12
            font.family: App.settings.font
            wrapMode: Text.WordWrap
            color: Material.secondaryTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            anchors {
                top: parent.top
                left: parent.left
                right: btnClear.left
                bottom: parent.bottom
                margins: 10
            }

        }

        Text {
            id: txtName

            clip: true
            font.pointSize: 12
            font.family: App.settings.font
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            color: Material.secondaryTextColor

            anchors {
                top: parent.top
                left: parent.left
                right: btnClear.left
                bottom: parent.bottom
                margins: 10
            }

        }

        Button {
            id: btnClear

            text: qsTr("Clear")
            Material.background: Material.theme === Material.Light ? Qt.lighter(Material.accent) : Qt.darker(Material.accent)
            Material.foreground: "white"
            onClicked: {
                root.file = "";
                fileDialog.file = "";
            }

            anchors {
                top: parent.top
                right: btnOpen.left
                bottom: parent.bottom
                margins: 5
            }

        }

        Button {
            id: btnOpen

            text: qsTr("Select File")
            Material.background: Material.accent
            Material.foreground: "white"
            font.family: App.settings.font
            onClicked: fileDialog.open()

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 10
                bottom: parent.bottom
                margins: 5
            }

        }

        FileDialog {
            id: fileDialog

            title: qsTr("Please choose a file")
            onAccepted: {
                root.file = fileDialog.currentFile ;
                txtName.text = fileDialog.currentFile.toString();
            }
        }

    }

    states: [
        State {
            name: "imageSelected"

            PropertyChanges {
                target: btnClear
                opacity: 1
                anchors.topMargin: 5
            }

            PropertyChanges {
                target: txtPlaceholder
                opacity: 0
            }

        },
        State {
            name: "nothingSelected"

            PropertyChanges {
                target: btnClear
                opacity: 0
                anchors.topMargin: -40
            }

        }
    ]
    transitions: [
        Transition {
            from: "imageSelected"
            to: "nothingSelected"
            reversible: true

            PropertyAnimation {
                target: btnClear
                properties: "opacity, anchors.topMargin"
                duration: 300
                easing.type: Easing.OutQuart
            }

            PropertyAnimation {
                target: txtPlaceholder
                property: "opacity"
                duration: 300
                easing.type: Easing.OutQuart
            }

        }
    ]
}
