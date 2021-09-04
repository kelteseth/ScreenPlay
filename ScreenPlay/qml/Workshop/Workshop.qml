import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Workshop 1.0
import ScreenPlay 1.0

Item {
    id: root

    ScreenPlayWorkshop {
        id: ws

        Component.onCompleted: {
            const success = ws.init();
            if(success){
                stackView.push("qrc:/qml/Workshop/SteamWorkshop.qml", {
                                   "workshop": ws,
                                   "steam": ws.steamWorkshop
                               })
            } else {
                popupOffline.open()
            }
        }
    }

    PopupOffline {
        id: popupOffline
        workshop: ws
        steam: ws.steamWorkshop
    }

    StackView {
        id: stackView

        property int duration: 300

        Connections {
            target: stackView.currentItem
            ignoreUnknownSignals: true
            function onOpenSteamProfile(){
                stackView.push("qrc:/qml/Workshop/SteamProfile.qml", {
                                      "workshop": ws,
                                      "steam": ws.steamWorkshop
                                  })
            }
            function onRequestWorkshopMainPage(){
                stackView.pop()
            }
        }

        anchors.fill: parent
        replaceEnter: Transition {
            OpacityAnimator {
                from: 0
                to: 1
                duration: stackView.duration
                easing.type: Easing.InOutQuart
            }

            ScaleAnimator {
                from: 0.8
                to: 1
                duration: stackView.duration
                easing.type: Easing.InOutQuart
            }
        }

        replaceExit: Transition {
            OpacityAnimator {
                from: 1
                to: 0
                duration: stackView.duration
                easing.type: Easing.InOutQuart
            }

            ScaleAnimator {
                from: 1
                to: 0.8
                duration: stackView.duration
                easing.type: Easing.InOutQuart
            }
        }
    }
}
