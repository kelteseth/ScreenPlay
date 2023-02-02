import QtQuick 2.15

Rectangle {
    anchors.fill: parent
    color: "black"

    Rectangle {
        id: toBeCreated
        anchors.fill: parent
        color: "black"
        opacity: 0

        Text {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            anchors.centerIn: parent
            text: qsTr("Please start ScreenPlay before launching the wallpaper")
            color: "White"
            font.pixelSize: 50
        }

        OpacityAnimator on opacity  {
            id: createAnimation
            from: 0
            to: 1
            duration: 1000
            onRunningChanged: {
                if (!running) {
                    toBeDeleted.opacity = 1;
                    toBeCreated.opacity = 0;
                    destroyAnimation.start();
                }
            }
        }
        Component.onCompleted: {
            createAnimation.start();
        }
    }

    Rectangle {
        id: toBeDeleted
        opacity: 0
        anchors.fill: parent
        color: "black"

        Text {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            anchors.centerIn: parent
            text: qsTr("Please start ScreenPlay before launching the wallpaper")
            color: "White"
            font.pixelSize: 50
        }

        OpacityAnimator on opacity  {
            id: destroyAnimation
            from: 1
            to: 0
            duration: 1000
            running: false
            onRunningChanged: {
                if (!running) {
                    toBeDeleted.opacity = 0;
                    toBeCreated.opacity = 0;
                    createAnimation.start();
                }
            }
        }
    }
}
