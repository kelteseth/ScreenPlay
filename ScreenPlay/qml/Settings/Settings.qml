import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import ScreenPlay 1.0
import ScreenPlay.Settings 1.0

import "../Common"

Item {
    id: root
    anchors.fill: parent

    function indexOfValue(model, value) {

        for (var i = 0; i < model.length; i++) {
            let ourValue = model[i].value
            if (value === ourValue)
                return i
        }
        return -1
    }

    Flickable {
        id: flickableWrapper
        width: 800
        height: parent.height
        contentHeight: columnWrapper.childrenRect.height
        contentWidth: 800
        flickableDirection: Flickable.VerticalFlick

        anchors {
            top: parent.top
            topMargin: 20
            bottom: parent.bottom
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
        Column {
            id: columnWrapper
            anchors.margins: 20
            width: 760
            spacing: 30

            Item {
                id: settingsGeneralWrapper
                height: 540 + txtDirChangesInfo.paintedHeight
                width: parent.width

                RectangularGlow {
                    id: effectBtnEmpty
                    anchors {
                        top: parent.top
                    }

                    height: parent.height
                    width: parent.width
                    cached: true
                    glowRadius: 3
                    spread: 0.2
                    color: "black"
                    opacity: 0.2
                    cornerRadius: 15
                }

                Rectangle {
                    anchors.fill: parent
                    radius: 4
                    clip: true

                    SettingsHeader {
                        id: headerGeneral
                        text: qsTr("General")
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }
                    }

                    Column {
                        spacing: 10
                        anchors {
                            top: headerGeneral.bottom
                            margins: 20
                            right: parent.right
                            bottom: parent.bottom
                            left: parent.left
                        }
                        SettingBool {
                            headline: qsTr("Autostart")
                            description: qsTr("ScreenPlay will start with Windows and will setup your Desktop every time for you.")
                            isChecked: ScreenPlay.settings.autostart
                            onCheckboxChanged: {
                                ScreenPlay.settings.setAutostart(checked)
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingBool {
                            headline: qsTr("High priority Autostart")
                            available: false

                            description: qsTr("This options grants ScreenPlay a higher autostart priority than other apps.")
                            isChecked: ScreenPlay.settings.highPriorityStart
                            onCheckboxChanged: {
                                ScreenPlay.settings.setHighPriorityStart(
                                            checked)
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingBool {
                            height: 70
                            headline: qsTr("Send anonymous crash reports and statistics")
                            description: qsTr("Help us make ScreenPlay faster and more stable. All collected data is purely anonymous and only used for development purposes!")
                            isChecked: ScreenPlay.settings.anonymousTelemetry
                            onCheckboxChanged: {
                                ScreenPlay.settings.setAnonymousTelemetry(
                                            checked)
                            }
                        }
                        SettingsHorizontalSeperator {}

                        SettingsButton {
                            headline: qsTr("Set save location")
                            description: {
                                // Remove file:/// so the used does not get confused
                                let path = ScreenPlay.globalVariables.localStoragePath + ""
                                if (path.length === 0) {
                                    return qsTr("Your storage path is empty!")
                                } else {
                                    return path.replace('file:///', '')
                                }
                            }

                            buttonText: qsTr("Set location")
                            onButtonPressed: {
                                folderDialogSaveLocation.open()
                            }
                            FolderDialog {
                                id: folderDialogSaveLocation
                                currentFolder: ScreenPlay.globalVariables.localStoragePath
                                onAccepted: {
                                    ScreenPlay.settings.setLocalStoragePath(
                                                folderDialogSaveLocation.currentFolder)
                                }
                            }
                        }

                        Text {
                            id: txtDirChangesInfo
                            text: qsTr("Important: Changing this directory has no effect on the workshop download path. ScreenPlay only supports having one content folder!")
                            color: "#B5B5B5"
                            height: 30
                            width: parent.width
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignLeft
                            font.pointSize: 10
                            font.family: ScreenPlay.settings.font
                            anchors {
                                right: parent.right
                                left: parent.left
                                leftMargin: 20
                            }
                        }

                        SettingsHorizontalSeperator {}

                        SettingsComboBox {
                            id: settingsLanguage
                            headline: qsTr("Language")
                            description: qsTr("Set the ScreenPlay UI Language")
                            Component.onCompleted: {
                                settingsLanguage.comboBox.currentIndex = root.indexOfValue(
                                            settingsLanguage.comboBox.model,
                                            ScreenPlay.settings.language)
                            }

                            comboBox {
                                onActivated: {
                                    ScreenPlay.settings.setLanguage(settingsLanguage.comboBox.currentValue)
                                    ScreenPlay.settings.retranslateUI()
                                }
                                model: [{
                                        "value": Settings.En,
                                        "text": qsTr("English")
                                    }, {
                                        "value":  Settings.De,
                                        "text": qsTr("German")
                                    }, {
                                        "value":  Settings.Ru,
                                        "text": qsTr("Russian")
                                    }, {
                                        "value":  Settings.Fr,
                                        "text": qsTr("French")
                                    }, {
                                        "value":  Settings.Es,
                                        "text": qsTr("Spanish")
                                    }, {
                                        "value":  Settings.Ko,
                                        "text": qsTr("Korean")
                                    }, {
                                        "value":  Settings.Vi,
                                        "text": qsTr("Vietnamese")
                                    }]
                            }
                        }
                    }
                }
            }

            Item {
                id: settingsPerformanceWrapper
                height: perfomanceWrapper.childrenRect.height + headerPerformance.height + 48
                width: parent.width

                RectangularGlow {
                    id: effect2
                    anchors {
                        top: parent.top
                    }

                    height: parent.height
                    width: parent.width
                    cached: true
                    glowRadius: 3
                    spread: 0.2
                    color: "black"
                    opacity: 0.2
                    cornerRadius: 15
                }

                Rectangle {
                    anchors.fill: parent
                    radius: 4
                    clip: true

                    SettingsHeader {
                        id: headerPerformance
                        text: qsTr("Performance")
                        image: "qrc:/assets/icons/icon_build.svg"
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }
                    }

                    Column {
                        id: perfomanceWrapper
                        anchors {
                            top: headerPerformance.bottom
                            margins: 20
                            right: parent.right
                            bottom: parent.bottom
                            left: parent.left
                        }
                        spacing: 10

                        SettingBool {
                            headline: qsTr("Pause wallpaper video rendering while another app is in the foreground")
                            description: qsTr("We disable the video rendering (not the audio!) for the best performance. If you have problem you can disable this behaviour here. Wallpaper restart required!")
                            isChecked: ScreenPlay.settings.checkWallpaperVisible
                            onCheckboxChanged: {
                                ScreenPlay.settings.setCheckWallpaperVisible(
                                            checked)
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingsComboBox {
                            id: cbVideoFillMode
                            headline: qsTr("Default Fill Mode")
                            description: qsTr("Set this property to define how the video is scaled to fit the target area.")
                            Component.onCompleted: {
                                cbVideoFillMode.comboBox.currentIndex = root.indexOfValue(
                                            cbVideoFillMode.comboBox.model,
                                            ScreenPlay.settings.videoFillMode)
                            }
                            comboBox {
                                onActivated: ScreenPlay.settings.setVideoFillMode(
                                                 cbVideoFillMode.comboBox.currentValue)

                                model: [{
                                        "value": Settings.Stretch,
                                        "text": qsTr("Stretch")
                                    }, {
                                        "value": Settings.Fill,
                                        "text": qsTr("Fill")
                                    }, {
                                        "value": Settings.Contain,
                                        "text": qsTr("Contain")
                                    }, {
                                        "value": Settings.Cover,
                                        "text": qsTr("Cover")
                                    }, {
                                        "value": Settings.Scale_Down,
                                        "text": qsTr("Scale-Down")
                                    }]
                            }
                        }
                    }
                }
            }

            Item {
                id: settingsAboutrapper
                height: settingsAboutrapperWrapper.childrenRect.height + 100
                width: parent.width

                RectangularGlow {
                    id: effect3
                    anchors {
                        top: parent.top
                    }

                    height: parent.height
                    width: parent.width
                    cached: true
                    glowRadius: 3
                    spread: 0.2
                    color: "black"
                    opacity: 0.2
                    cornerRadius: 15
                }

                Rectangle {

                    anchors.fill: parent
                    radius: 4
                    clip: true

                    SettingsHeader {
                        id: headerAbout
                        text: qsTr("About")
                        image: "qrc:/assets/icons/icon_cake.svg"
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }
                    }

                    Column {
                        id: settingsAboutrapperWrapper
                        width: parent.width
                        spacing: 10
                        anchors {
                            top: headerAbout.bottom
                            left: parent.left
                            right: parent.right
                            margins: 20
                        }

                        Item {
                            width: parent.width
                            height: 180
                            Text {
                                id: txtHeadline
                                color: "#5D5D5D"
                                text: qsTr("Thank you for using ScreenPlay")

                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                font.pointSize: 16
                                 font.family: ScreenPlay.settings.font
                                anchors {
                                    top: parent.top
                                    topMargin: 6
                                    left: parent.left
                                    leftMargin: 20
                                }
                            }
                            Text {
                                id: txtDescriptionAbout
                                text: qsTr("Hi, I'm Elias Steurer also known as Kelteseth and I'm the developer of ScreenPlay. Thank you for using my software. You can follow me to receive updates about ScreenPlay here:")
                                color: "#B5B5B5"

                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignTop
                                horizontalAlignment: Text.AlignLeft
                                font.pointSize: 11
                                font.family: ScreenPlay.settings.font
                                width: parent.width * .6
                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: 15
                                    left: parent.left
                                    leftMargin: 20
                                    right: imgLogoHead.left
                                    rightMargin: 60
                                    bottom: parent.bottom
                                }
                            }

                            RowLayout {
                                anchors {
                                    left: parent.left
                                    margins: 20
                                    bottom: parent.bottom
                                }
                                spacing: 20

                                GrowIconLink {
                                    iconSource: "qrc:/assets/icons/brand_github.svg"
                                    url: "https://github.com/kelteseth"
                                    color: "#333333"
                                }
                                GrowIconLink {
                                    iconSource: "qrc:/assets/icons/brand_gitlab.svg"
                                    url: "https://gitlab.com/kelteseth"
                                    color: "#FC6D26"
                                }
                                GrowIconLink {
                                    iconSource: "qrc:/assets/icons/brand_twitter.svg"
                                    url: "https://twitter.com/Kelteseth"
                                    color: "#1DA1F2"
                                }
                                GrowIconLink {
                                    iconSource: "qrc:/assets/icons/brand_twitch.svg"
                                    url: "https://www.twitch.tv/kelteseth/"
                                    color: "#6441A5"
                                }
                            }

                            Image {
                                id: imgLogoHead
                                source: "https://assets.gitlab-static.net/uploads/-/system/user/avatar/64172/avatar.png"

                                width: 120
                                height: 120
                                visible: false
                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: 0
                                    right: parent.right
                                    rightMargin: 20
                                }
                                sourceSize: Qt.size(width, height)
                            }
                            Image {
                                id: mask
                                source: "qrc:/assets/images/mask_round.svg"
                                sourceSize: Qt.size(width, height)
                                smooth: true
                                width: 120
                                height: 120
                                visible: false
                            }

                            OpacityMask {
                                id: opacityMask
                                anchors.fill: imgLogoHead
                                source: imgLogoHead
                                maskSource: mask
                                smooth: true
                            }
                        }
                        SettingsHorizontalSeperator {}

                        SettingsButton {
                            icon.source: "qrc:/assets/icons/icon_launch.svg"
                            headline: qsTr("Version")
                            description: qsTr("ScreenPlay Build Version ")
                                         + ScreenPlay.settings.gitBuildHash
                            buttonText: qsTr("Open Changelog")
                            onButtonPressed: Qt.openUrlExternally(
                                                 "https://gitlab.com/kelteseth/ScreenPlay/-/releases")
                        }

                        SettingsHorizontalSeperator {}
                        SettingsButton {
                            headline: qsTr("Third Party Software")
                            description: qsTr("ScreenPlay would not be possible without the work of others. A big thank you to: ")
                            buttonText: qsTr("Licenses")
                            onButtonPressed: {
                                ScreenPlay.util.requestAllLicenses()
                                expanderCopyright.toggle()
                            }
                        }
                        SettingsExpander {
                            id: expanderCopyright
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            Connections {
                                target: ScreenPlay.util
                                function onAllLicenseLoaded(licensesText) {
                                    expanderCopyright.text = licensesText
                                }
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingsButton {
                            headline: qsTr("Debug Messages")
                            description: qsTr("If your ScreenPlay missbehaves this is a good way to look for answers. This shows all logs and warning during runtime.")
                            buttonText: qsTr("Debug Messages")
                            onButtonPressed: {
                                expanderDebug.toggle()
                            }
                        }
                        SettingsExpander {
                            id: expanderDebug
                            text: ScreenPlay.util.debugMessages
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingsButton {
                            headline: qsTr("Data Protection")
                            description: qsTr("We use you data very carefully to improve ScreenPlay. We do not sell or share this (anonymous) information with others!")
                            buttonText: qsTr("Privacy")
                            onButtonPressed: {
                                ScreenPlay.util.requestDataProtection()
                                expanderDataProtection.toggle()
                            }
                        }
                        SettingsExpander {
                            id: expanderDataProtection
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            Connections {
                                target: ScreenPlay.util
                                function onAllDataProtectionLoaded(dataProtectionText) {
                                    expanderDataProtection.text = dataProtectionText
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

