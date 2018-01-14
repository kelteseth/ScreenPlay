import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: monitors
    state: "inactive"
    focus: true

    property string activeMonitorName: ""

    onStateChanged: {
        bgMouseArea.focus = monitors.state == "active" ? true : false
        if (monitors.state == "active") {


            //monitorListModel.reloadMonitors();
        }
    }

    Rectangle {
        id: background
        color: "#99000000"
        anchors.fill: parent

        MouseArea {
            id: bgMouseArea
            anchors.fill: parent
            onClicked: monitors.state = "inactive"
        }
    }

    Rectangle {
        color: "white"
        width: 900
        radius: 3
        height: 600
        z: 98
        anchors.centerIn: parent

        MouseArea {
            anchors.fill: parent
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            MonitorSelection {
                id: monitorSelection
                background: "#eeeeee"
                radius: 3
                height: 200
                width: 800
                Layout.fillWidth: true
                availableWidth: 700
                availableHeight: 150
            }

            Item {
                id: sliderVolumeWrapper
                height: 100
                width: 320
                Layout.alignment: Qt.AlignHCenter
                Text {
                    id: txtSliderVolume
                    text: qsTr("Volume")
                    height: 30
                    renderType: Text.NativeRendering
                    font.family: "Roboto"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                    color: "#626262"
                    wrapMode: Text.WrapAnywhere
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                }
                Row {
                    id: sliderVolumeWrapperBottom
                    height: 70
                    width: parent.width
                    spacing: 30
                    anchors {
                        top: txtSliderVolume.bottom
                        topMargin: 5
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }

                    Image {
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/assets/icons/icon_volume.svg"
                        sourceSize: Qt.size(20, 20)
                    }

                    Slider {
                        id: sliderVolume
                        stepSize: 0.01
                        from: 0
                        live: true
                        value: .8
                        to: 1
                        anchors.verticalCenter: parent.verticalCenter
                        Layout.fillWidth: true
                        onValueChanged: {
                            screenPlaySettings.setGlobalVolume(
                                        sliderVolume.value)
                        }
                    }

                    Text {
                        id: name
                        color: "#818181"
                        text: Math.round(sliderVolume.value * 100) / 100
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: "Libre Baskerville"
                        font.pointSize: 12
                        font.italic: true
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            ComboBox {
                id: comboBox
                width: 200
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 200

                model: ListModel {
                    id: model
                    ListElement {
                        text: "Stretch"
                    }
                    ListElement {
                        text: "PreserveAspectFit"
                    }
                    ListElement {
                        text: "PreserveAspectCrop"
                    }
                }

                onActivated: screenPlaySettings.setGlobalFillMode(currentText)
            }

            Button {
                id: btnRemoveAllWallpaper
                text: qsTr("Remove all wallpaper")
                Material.background: Material.Orange
                Material.foreground: "white"
                enabled: screenPlaySettings.activeWallpaperCounterChanged === 0 ? false : true
                onClicked: screenPlaySettings.removeAll(
                               ) //screenPlaySettings.removeWallpaperAt(monitorSelection.activeMonitorIndex)
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: monitors
                opacity: 1
                enabled: true
            }

            PropertyChanges {
                target: background
                opacity: 1
            }
        },
        State {
            name: "inactive"
            PropertyChanges {
                target: monitors
                opacity: 0
                enabled: false
            }

            PropertyChanges {
                target: background
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {

            NumberAnimation {
                target: background
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: monitors
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
