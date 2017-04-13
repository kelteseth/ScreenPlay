import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: monitors
    state:"inactive"

    property string activeMonitorName: ""

    Component.onCompleted: {

        var a = monitorList.get();
        print(a[0]);

    }

    Rectangle {
        id:blurParent
        color: "#80ffffff"
        anchors.fill: parent
    }


    Rectangle {
        width: 800
        height: 600

        anchors.centerIn: parent
    }


    FastBlur {
        id:blur
        anchors.fill: monitors
        source: blurParent
        radius: 64
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: monitors
                visible: true
            }

            PropertyChanges {
                target: blur
                opacity: 1
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: monitors
                visible: false
            }

            PropertyChanges {
                target: blur
                opacity: 0
            }
        }
    ]

}
