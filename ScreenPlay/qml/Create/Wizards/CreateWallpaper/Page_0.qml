import QtQuick 2.0
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    property bool allNecessaryConfigsSet: false



    Rectangle {
        id: rectangle1
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        RectangularGlow {
            id: effect
            anchors {
                top: imgWrapper.top
                left: imgWrapper.left
                right: imgWrapper.right
                topMargin: 3
            }

            height: imgWrapper.height
            width: imgWrapper.width
            cached: true
            glowRadius: 3
            spread: 0.2
            color: "black"
            opacity: 0.4
            cornerRadius: 15
        }


        Rectangle {
            id: imgWrapper
            width: parent.width * .9
            anchors {
                top:parent.top
                margins: parent.width * .05
                right:parent.right
                left:parent.left
            }

            height: 200
            color: "gray"
            Image {
                id: imgPreview
            }
        }
    }

    Rectangle {
        id: rectangle
        width: parent.width * .5
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        Column {
            id: column
            spacing: 20
            anchors.fill: parent
            anchors.margins: 30

            TextField {
                id: textField
                placeholderText: qsTr("Name")
                anchors.right: parent.right
                anchors.left: parent.left

            }

            TextField {
                id: textField1
                placeholderText: qsTr("Description")
                anchors.right: parent.right
                anchors.left: parent.left
            }

            TextField {
                id: textField2
                placeholderText: qsTr("Youtube URL")
                anchors.right: parent.right
                anchors.left: parent.left
            }

            TextField {
                id: textField3
                placeholderText: qsTr("Tags")
                anchors.right: parent.right
                anchors.left: parent.left
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:768;width:1366}D{i:4;anchors_height:200;anchors_x:200;anchors_y:0}
D{i:7;anchors_height:400;anchors_width:200}D{i:3;anchors_height:200;anchors_width:683}
}
 ##^##*/
