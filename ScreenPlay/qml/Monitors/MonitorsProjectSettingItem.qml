import QtQuick 2.12
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import ScreenPlay 1.0
Item {
    id: delegate
    focus: true
    height: isHeadline ? 50 : 30
    width: 300

    property int selectedMonitor

    anchors {
        left: parent.left
        leftMargin: isHeadline ? 0 : 25
    }

    Text {
        id: txtDescription
        text: name
        width: 100
        font.pointSize: isHeadline ? 21 : 14
        anchors.verticalCenter: parent.verticalCenter
        font.family: "Roboto"
        font.weight: Font.Normal
        
        anchors {
            left: parent.left
        }
    }

    Item {
        height: parent.height
        visible: !isHeadline
        anchors {
            left: txtDescription.right
            leftMargin: 20
            right: parent.right
        }

        Component.onCompleted: {
            if (value.toString() === "") {
                return
            }

            var obj = JSON.parse(value.toString())

            if (obj["type"] === "slider") {
                loader.sourceComponent = compSlider
                loader.item.from = obj["from"]
                loader.item.to = obj["to"]
                loader.item.value = obj["value"]
                loader.item.stepSize = obj["stepSize"]
            }
        }
        Component {
            id: compSlider

            Item {
                id:root
                anchors.fill: parent
                property int from
                property int to
                property int value
                property int stepSize

                Slider {
                    id:slider
                    from: root.from
                    to: root.to
                    value: root.value
                    stepSize: root.stepSize
                    live: false
                    width: parent.width

                    onValueChanged: {
                        var value = Math.round(slider.value * 100) / 100;
                        txtSliderValue.text = value;
                        ScreenPlay.screenPlayManager.setWallpaperValue(selectedMonitor,txtDescription.text,value)
                    }
                }
                Text {
                    id: txtSliderValue
                    anchors{
                        right:parent.right
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Loader {
            id: loader
            anchors.fill: parent
        }
    }
}
