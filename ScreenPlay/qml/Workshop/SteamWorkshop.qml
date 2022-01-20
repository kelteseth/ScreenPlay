import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Workshop 1.0
import WorkshopEnums 1.0
import ScreenPlay 1.0
import "upload/"

Item {
    id: root

    required property Item modalSource

    ScreenPlayWorkshop {
        id: screenPlayWorkshop
        Component.onCompleted: {
            if (screenPlayWorkshop.init()) {
                stackView.push("qrc:/ScreenPlay/qml/Workshop/SteamWorkshopStartPage.qml", {
                                   "stackView": stackView,
                                   "screenPlayWorkshop": screenPlayWorkshop,
                                   "steamWorkshop": screenPlayWorkshop.steamWorkshop,
                                   "background": background,
                                   "modalSource": root.modalSource
                               })
            } else {
                popupOffline.open()
            }
        }
    }


    Background {
        id: background
        anchors.fill: parent
        stackViewDepth: stackView.depth
    }


    PopupOffline {
        id: popupOffline
        workshop: screenPlayWorkshop
        steam: screenPlayWorkshop.steamWorkshop
        modalSource: root.modalSource
    }

    StackView {
        id: stackView
        property int duration: 300

        Connections {
            target: stackView.currentItem
            ignoreUnknownSignals: true
            function onRequestBack() {
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
