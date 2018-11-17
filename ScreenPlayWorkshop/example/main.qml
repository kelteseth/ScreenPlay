import QtQuick 2.9
import QtQuick.Window 2.2
import net.aimber.workshop 1.0

Window {
    visible: true
    width: 640
    height: 360
    title: qsTr("Workshop Example Widget")

    Component.onCompleted: {

    }

    Workshop {
        id: test
    }
}
