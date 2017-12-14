import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    anchors.fill: parent

    Feedback {
        id:feedback
        width: parent.width * .35
        height: parent.height
        anchors.left: parent.left
    }

    ScrollView {
        anchors {
            top:parent.top
            right: parent.right
            bottom:parent.bottom
            left: feedback.left
        }


    }

}
