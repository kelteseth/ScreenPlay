import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import ScreenPlay

import ScreenPlayCore

Item {
    id: root
    objectName: "createWallpaperInit"

    property int quality: sliderQuality.slider.value

    signal next(var codec, var quality)

    ColumnLayout {
        spacing: 40

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 20
        }

        Headline {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            text: qsTr("Import Video Wallpaper - Select Codec")
        }

        Text {
            id: txtDescription

            text: qsTr("Depending on your PC configuration it is better to convert your wallpaper to a specific video codec. We skip encoding if the input format matches the ouput format.")
            color: Material.primaryTextColor
            Layout.fillWidth: true
            font.pointSize: 13
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: App.settings.font
        }

        ColumnLayout {
            spacing: 20

            Text {
                id: txtComboboxHeadline

                text: qsTr("Set your preffered video codec:")
                color: Material.primaryTextColor
                width: parent.width
                font.pointSize: 14
                font.family: App.settings.font
            }

            ComboBox {
                id: comboBoxCodec

                Layout.preferredWidth: 400
                textRole: "text"
                valueRole: "value"
                currentIndex: 0
                font.family: App.settings.font

                model: ListModel {
                    id: model
                    ListElement {
                        value: Video.VideoCodec.H264
                        text: qsTr("✨h.264 (Better for all hardware)")
                    }

                    ListElement {
                        value: Video.VideoCodec.VP9
                        text: qsTr("VP9 (Better for newer hardware 2018+)")
                    }

                    ListElement {
                        value: Video.VideoCodec.VP8
                        text: qsTr("VP8 (Better for older hardware)")
                    }
                }
            }
        }

        LabelSlider {
            id: sliderQuality

            iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_settings.svg"
            headline: qsTr("Set video quality. Lower value means better quality.")
            Layout.preferredWidth: 400

            slider {
                from: 63
                value: 22
                to: 0
                stepSize: 1
            }
        }
    }

    Button {
        text: qsTr("Open Documentation")
        Material.accent: Material.color(Material.LightGreen)
        highlighted: true
        icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_document.svg"
        icon.color: "white"
        icon.width: 16
        icon.height: 16
        font.family: App.settings.font
        onClicked: Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance")

        anchors {
            bottom: parent.bottom
            margins: 20
        }
    }

    Button {
        objectName: "createWallpaperStartImportButton"
        text: qsTr("Start import")
        highlighted: true
        font.family: App.settings.font
        onClicked: {
            root.next(comboBoxCodec.currentValue, sliderQuality.value);
        }
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
    }
}
