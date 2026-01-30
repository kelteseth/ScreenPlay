import QtQuick
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
    property var detectedCodec: Video.VideoCodec.Unknown
    property string videoPath: ""

    signal next(var codec, var quality)

    Component.onCompleted: {
        if (videoPath !== "") {
            detectedCodec = App.create.detectVideoCodec(videoPath)
            updateCodecSelection()
        }
    }

    function updateCodecSelection() {
        // Check if conversion can be skipped for the detected codec
        const canSkip = App.create.canSkipConversion(detectedCodec)

        if (canSkip) {
            // Codec is playable, suggest "No Conversion"
            comboBoxCodec.currentIndex = 0;
            // No Conversion
            txtDetectedCodec.text = qsTr("✅ Your video codec (%1) is supported! No conversion needed.").arg(codecName(detectedCodec))
            txtDetectedCodec.visible = true
        } else if (detectedCodec !== Video.VideoCodec.Unknown) {
            // Codec detected but not playable, suggest conversion to detected codec
            for (let i = 0; i < model.count; i++) {
                if (model.get(i).value === detectedCodec) {
                    comboBoxCodec.currentIndex = i
                    break
                }
            }
            txtDetectedCodec.visible = false
        } else {
            // Unknown codec, default to H.264
            txtDetectedCodec.visible = false
        }
    }

    function codecName(codec) {
        switch (codec) {
        case Video.VideoCodec.H264:
            return "H.264"
        case Video.VideoCodec.H265:
            return "H.265"
        case Video.VideoCodec.VP8:
            return "VP8"
        case Video.VideoCodec.VP9:
            return "VP9"
        case Video.VideoCodec.AV1:
            return "AV1"
        default:
            return "Unknown"
        }
    }

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

        Text {
            id: txtDetectedCodec
            visible: false
            text: qsTr("✅ Your video codec is supported! No conversion needed.")
            color: Material.color(Material.Green)
            Layout.fillWidth: true
            font.pointSize: 12
            font.bold: true
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

                onCurrentValueChanged: {
                    // Disable quality slider when NoConversion is selected
                    sliderQuality.enabled = (currentValue !== Video.VideoCodec.NoConversion)
                }

                model: ListModel {
                    id: model
                    ListElement {
                        value: Video.VideoCodec.NoConversion
                        text: qsTr("🎬 No Conversion (Keep original)")
                    }
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

            iconSource: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_settings.svg"
            headline: qsTr("Set video quality. Lower value means better quality.")
            Layout.preferredWidth: 400
            enabled: comboBoxCodec.currentValue !== Video.VideoCodec.NoConversion
            opacity: enabled ? 1.0 : 0.5

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
        icon.source: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_document.svg"
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
            root.next(comboBoxCodec.currentValue, sliderQuality.slider.value)
        }
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }
    }
}
