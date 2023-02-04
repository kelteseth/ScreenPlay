import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWorkshop

Window {
    id: root
    width: 1400
    height: 768
    visible: true
    title: qsTr("ScreenPlayWorkshop")
    Component.onCompleted: {
        root.Material.theme = Material.Dark;
    }

    ScreenPlayWorkshop {
        id: screenPlayWorkshop
        Component.onCompleted: {
            if (screenPlayWorkshop.init()) {
                print("init");
                stackView.push("qrc:/qml/ScreenPlayWorkshop/qml/SteamWorkshopStartPage.qml", {
                        "stackView": stackView,
                        "screenPlayWorkshop": screenPlayWorkshop,
                        "steamWorkshop": screenPlayWorkshop.steamWorkshop,
                        "background": background
                    });
            } else {
                popupOffline.open();
            }
        }
    }

    Item {
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
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
        }
        StackView {
            id: stackView
            property int duration: 300

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
    Rectangle {
        id: nav
        height: 60
        color: Material.background
        anchors {
            right: parent.right
            left: parent.left
        }
    }
}
