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
    property var videoInfo: ({})

    signal next(var codec, var quality)

    SwipeView.onIsCurrentItemChanged: {
        if (!SwipeView.isCurrentItem)
            return
        if (videoPath === "") {
            console.error("CreateWallpaperSettings: videoPath is empty!")
            return
        }
        App.create.probeVideoInfo(videoPath).then(result => {
            videoInfo = result
            if (result.hasOwnProperty("detectedCodec")) {
                detectedCodec = result.detectedCodec
            }
            updateCodecSelection()
        })
    }

    function updateCodecSelection() {
        const canSkip = App.create.canSkipConversion(detectedCodec)

        if (canSkip) {
            // H.264, VP8, VP9 are natively supported — suggest skipping conversion
            comboBoxCodec.currentIndex = 0
            txtDetectedCodec.text = qsTr("✅ Your video codec (%1) is supported! No conversion needed.").arg(codecName(detectedCodec))
            txtDetectedCodec.visible = true
        } else {
            // All other codecs (H.265, AV1, MJPEG, Unknown) — default to H.264
            for (let i = 0; i < model.count; i++) {
                if (model.get(i).value === Video.VideoCodec.H264) {
                    comboBoxCodec.currentIndex = i
                    break
                }
            }
            if (detectedCodec !== Video.VideoCodec.Unknown) {
                txtDetectedCodec.text = qsTr("⚠️ Your video codec (%1) requires conversion for best compatibility.").arg(codecName(detectedCodec))
                txtDetectedCodec.visible = true
            } else {
                txtDetectedCodec.visible = false
            }
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
        case Video.VideoCodec.MJPEG:
            return "MJPEG"
        default:
            return "Unknown"
        }
    }

    ColumnLayout {
        spacing: 20

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: btnDocumentation.top
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

        RowLayout {
            spacing: 20
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 20
                Layout.preferredWidth: 420
                Layout.maximumWidth: 420
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignTop

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

                    function qualityLabel(v) {
                        if (v <= 5)
                            return qsTr("Quality: Indistinguishable from original (0-5)")
                        if (v <= 15)
                            return qsTr("Quality: Very good (6-15)")
                        if (v <= 28)
                            return qsTr("Quality: Good (16-28)")
                        if (v <= 40)
                            return qsTr("Quality: Acceptable (29-40)")
                        return qsTr("Quality: Low (41-63)")
                    }

                    iconSource: "qrc:/qt/qml/ScreenPlayCore/assets/icons/icon_settings.svg"
                    headline: qualityLabel(slider.value)
                    Layout.preferredWidth: 400
                    enabled: comboBoxCodec.currentValue !== Video.VideoCodec.NoConversion
                    opacity: enabled ? 1.0 : 0.25

                    slider {
                        from: 63
                        value: 10
                        to: 0
                        stepSize: 1
                    }
                }
            } // end left ColumnLayout

            // Video Stats Panel
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 200
                color: Qt.rgba(1, 1, 1, 0.05)
                radius: 8
                visible: Object.keys(root.videoInfo).length > 0

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 0

                    Text {
                        text: qsTr("Video Information")
                        color: Material.primaryTextColor
                        font.pointSize: 14
                        font.bold: true
                        font.family: App.settings.font
                        Layout.fillWidth: true
                        Layout.bottomMargin: 12
                    }

                    Flickable {
                        id: statsFlickable
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        contentHeight: statsColumn.height
                        clip: true
                        boundsBehavior: Flickable.StopAtBounds

                        ScrollBar.vertical: ScrollBar {
                            policy: statsFlickable.contentHeight > statsFlickable.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                        }

                        Column {
                            id: statsColumn
                            width: parent.width
                            spacing: 2

                            Repeater {
                                // Ordered key list for consistent display
                                model: {
                                    const orderedKeys = ["Container", "Video Codec", "Resolution", "Frame Rate", "Pixel Format", "Profile", "Level", "Video Bitrate", "Total Frames", "Duration", "Audio Codec", "Sample Rate", "Audio Channels", "Audio Bitrate", "Audio", "File Size"]
                                    let result = []
                                    for (let key of orderedKeys) {
                                        if (root.videoInfo.hasOwnProperty(key))
                                            result.push({
                                                key: key,
                                                value: root.videoInfo[key]
                                            })
                                    }
                                    return result
                                }

                                delegate: Column {
                                    width: statsColumn.width
                                    topPadding: 6
                                    bottomPadding: 6

                                    Text {
                                        text: modelData.key
                                        color: Material.secondaryTextColor
                                        font.pointSize: 10
                                        font.family: App.settings.font
                                        width: parent.width
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        text: modelData.value
                                        color: Material.primaryTextColor
                                        font.pointSize: 12
                                        font.family: App.settings.font
                                        width: parent.width
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    }

                                    Rectangle {
                                        width: parent.width
                                        height: 1
                                        color: Qt.rgba(1, 1, 1, 0.08)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } // end RowLayout
    } // end ColumnLayout

    Button {
        id: btnDocumentation
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
