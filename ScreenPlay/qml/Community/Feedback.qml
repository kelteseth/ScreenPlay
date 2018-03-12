import QtQuick 2.9
import com.stomt.qmlSDK 1.0

Item {
    height: 236
    width: 420
    StomtWidget {
        id:stomtWidget
        anchors {
            top:parent.top
            topMargin: 30
            horizontalCenter: parent.horizontalCenter
        }
        targetID: "screenplay"
        appKey: "C1IfJAruqWFdkM8e7BMPK3dx1"
        targetDisplayName: "ScreenPlay"
        targetImageUrl: "qrc:/assets/images/ScreenPlayIconStomt.png"

    }
}


