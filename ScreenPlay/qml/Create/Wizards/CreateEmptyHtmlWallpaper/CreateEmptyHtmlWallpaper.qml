import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: root

    SwipeView {
        id: swipeView
        anchors.fill: parent
        anchors.margins: 20
        interactive: false
        clip: true

        Item {
            Item {
                width: parent.width * .66
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                }
                Image {
                    source: "qrc:/assets/images/undraw_static_website_0107.svg"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    width: parent.width
                    height: 300
                    fillMode: Image.PreserveAspectFit

                }
            }

            Item {
                width: parent.width * .33
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }

                Rectangle {
                    anchors.centerIn: parent
                    width: 100
                    height: width
                    color: "orange"
                }
            }

            Button {
                text: qsTr("Next")
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }

                onClicked: swipeView.incrementCurrentIndex()
            }
        }
        Item {

            Rectangle {
                anchors.centerIn: parent
                width: 100
                height: width
                color: "steelblue"
            }
            Button {
                text: qsTr("Next")
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }
                onClicked: swipeView.incrementCurrentIndex()
            }
        }
        Item {

            Rectangle {
                anchors.centerIn: parent
                width: 100
                height: width
                color: "gray"
            }
            Button {
                text: qsTr("Finish")
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }
                onClicked: swipeView.incrementCurrentIndex()
            }
        }
    }

    PageIndicator {
        id: indicator

        count: swipeView.count
        currentIndex: swipeView.currentIndex

        anchors.bottom: swipeView.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
