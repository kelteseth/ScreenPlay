import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import ScreenPlay 1.0
import ScreenPlay.Create 1.0
import "../../../Common" as Common

Item {
    id: root
    objectName: "createWallpaperInit"

    property int quality: sliderQuality.slider.value

    signal next(var filePath, var codec)

    ColumnLayout {
        spacing: 40

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 20
        }

        Common.Headline {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            text: qsTr("Import any video type")
        }

        Text {
            id: txtDescription

            text: qsTr("Depending on your PC configuration it is better to convert your wallpaper to a specific video codec. If both have bad performance you can also try a QML wallpaper! Supported video formats are: \n
*.mp4  *.mpg *.mp2 *.mpeg *.ogv *.avi *.wmv *.m4v *.3gp *.flv")
            color: Material.primaryTextColor
            Layout.fillWidth: true
            font.pointSize: 13
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: ScreenPlay.settings.font
        }

        ColumnLayout {
            spacing: 20

            Text {
                id: txtComboboxHeadline

                text: qsTr("Set your preffered video codec:")
                color: Material.primaryTextColor
                width: parent.width
                font.pointSize: 14
                font.family: ScreenPlay.settings.font
            }

            ComboBox {
                //                ListElement {
                //                    text: "AV1 (NVidia 3000, AMD 6000 or newer). ULTRA SLOW ENCODING!"
                //                    value: Create.AV1
                //                }

                id: comboBoxCodec

                Layout.preferredWidth: 400
                textRole: "text"
                valueRole: "value"
                currentIndex: 1
                font.family: ScreenPlay.settings.font

                model: ListModel {
                    id: model

                    ListElement {
                        text: "VP8 (Better for older hardware)"
                        value: Create.VP9
                    }

                    ListElement {
                        text: "VP9 (Better for newer hardware 2018+)"
                        value: Create.VP8
                    }
                    // Import works but the QWebEngine cannot display AV1 :(

                }

            }

        }

        Common.Slider {
            id: sliderQuality

            iconSource: "qrc:/assets/icons/icon_settings.svg"
            headline: qsTr("Quality slider. Lower value means better quality.")
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
        Material.background: Material.LightGreen
        Material.foreground: "white"
        icon.source: "qrc:/assets/icons/icon_document.svg"
        icon.color: "white"
        icon.width: 16
        icon.height: 16
        font.family: ScreenPlay.settings.font
        onClicked: Qt.openUrlExternally("https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/#performance")

        anchors {
            bottom: parent.bottom
            margins: 20
        }

    }

    Button {
        objectName: "createWallpaperInitFileSelectButton"
        text: qsTr("Select file")
        highlighted: true
        font.family: ScreenPlay.settings.font
        onClicked: {
            fileDialogImportVideo.open();
        }

        FileDialog {
            id: fileDialogImportVideo

            nameFilters: ["Video files (*.mp4  *.mpg *.mp2 *.mpeg *.ogv *.avi *.wmv *.m4v *.3gp *.flv)"]
            onAccepted: {
                root.next(fileDialogImportVideo.fileUrl, model.get(comboBoxCodec.currentIndex).value);
            }
        }

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }

    }

}
