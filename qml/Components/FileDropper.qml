import QtQuick 2.9
import QtQuick.Extras 1.4
import PackageFileHandler 1.0

DropArea {
    id:dropper
    state:"invisible"

    onEntered: {
        dropper.state = "visible"
    }

    onExited: {
        dropper.state = "invisible"
    }

    Connections {
        target: packageFileHandler
        onCurrentLoaderStatusChanged:print("aaaaa")
    }

    onDropped: {
        var a= packageFileHandler.loadPackageFromLocalZip(drop.urls,".")
        print(packageFileHandler.currentLoaderStatus)

        if(a === -1) {
            dropper.state = "badFile"
            timer.start()
        } else if(a === 0){
            dropper.state = "invisible"
        }

    }

    Timer {
        id:timer
        interval: 1000
        onTriggered: {
            dropper.state = "invisible"
            timer.stop()
        }
    }


    Rectangle {
        id:fill
        anchors.fill: parent
        opacity: .8

        Picture {
            id: picture
            x: 332
            y: 208
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc://assets/icons/icon_settings.svg"
        }
    }

    Text {
        id: text1
        text: qsTr("Bad File!!")
        font.pixelSize: 24
        opacity: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    states: [
        State {
            name: "visible"
        },
        State {
            name: "invisible"

            PropertyChanges {
                target: fill
                opacity: 0
            }

            PropertyChanges {
                target: text1
                opacity: 0
            }
        },
        State {
            name: "badFile"
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "invisible"

            NumberAnimation {
                target: fill
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "invisible"
            to: "visible"

            NumberAnimation {
                target: fill
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]

}
