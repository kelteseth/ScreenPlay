import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

import ScreenPlay 1.0

import "../Common"

Item {
    id: settingsWrapper
    anchors.fill: parent

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
                                ScreenPlay.settings.writeSingleSettingConfig(
                                            "autostart", checked)
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
                                ScreenPlay.settings.writeSingleSettingConfig(
                                            "highPriorityStart", checked)
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingBool {
                            height: 70
                            headline: qsTr("Send anonymous crash reports and statistics")
                            description: qsTr("Help us make ScreenPlay faster and more stable. All collected data is purely anonymous and only used for development purposes!")
                            isChecked: ScreenPlay.settings.anonymousTelemetry
                            onCheckboxChanged: {
                                ScreenPlay.settings.setAnonymousTelemetry(checked)
                                ScreenPlay.settings.writeSingleSettingConfig(
                                            "anonymousTelemetry", checked)
                            }
                        }
                        SettingsHorizontalSeperator {}

                        SettingsButton {
                            headline: qsTr("Set save location")
                            description: ScreenPlay.globalVariables.localStoragePath //qsTr("Choose where to find you content. The default save location is you steam installation")
                            buttonText: qsTr("Set location")
                            onButtonPressed: {
                                folderDialogSaveLocation.open()
                            }
                            FolderDialog {
                                id: folderDialogSaveLocation
                                currentFolder: ScreenPlay.globalVariables.localStoragePath
                                onAccepted: {
                                    ScreenPlay.globalVariables.setLocalStoragePath(
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
                            font.family: "Roboto"
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
                            onCurrentIndexChanged: {
                                var key = settingsLanguage.comboBoxListModel.get(
                                            settingsLanguage.currentIndex).text.toString()

                                var languageKey

                                switch (key) {
                                case "German":
                                    languageKey = "de"
                                    break
                                case "English":
                                    languageKey = "en"
                                    break
                                case "Russian":
                                    languageKey = "ru"
                                    break
                                case "French":
                                    languageKey = "fr"
                                    break
                                case "Spanish":
                                    languageKey = "es"
                                    break
                                default:
                                    languageKey = "en"
                                    break
                                }

                                ScreenPlay.settings.setqSetting("language",
                                                                languageKey)

                                ScreenPlay.settings.setupLanguage()
                                ScreenPlay.mainWindowEngine.retranslate()
                            }
                            comboBoxListModel: ListModel {
                                ListElement {
                                    text: "English"
                                }
                                ListElement {
                                    text: "German"
                                }
                                ListElement {
                                    text: "Russian"
                                }
                                ListElement {
                                    text: "French"
                                }
                                ListElement {
                                    text: "Spanish"
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: settingsPerformanceWrapper
                height: 260
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
                        anchors {
                            top: headerPerformance.bottom
                            margins: 20
                            right: parent.right
                            bottom: parent.bottom
                            left: parent.left
                        }
                        spacing: 10

                        SettingBool {
                            headline: qsTr("Pause wallpaper while ingame")
                            available: false
                            description: qsTr("To maximise your framerates ingame, you can enable this setting to pause all active wallpapers!")
                            isChecked: ScreenPlay.settings.pauseWallpaperWhenIngame
                            onCheckboxChanged: {
                                ScreenPlay.settings.setPauseWallpaperWhenIngame(
                                            checked)
                                ScreenPlay.settings.writeSingleSettingConfig(
                                            "setPauseWallpaperWhenIngame",
                                            checked)
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingsComboBox {
                            id: settingsFillModeComboBox
                            headline: qsTr("Default Fill Mode")
                            description: qsTr("Set this property to define how the video is scaled to fit the target area.")
                            onCurrentIndexChanged: {

                            }
                            comboBoxListModel: ListModel {
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
                                font.family: "Roboto"
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
                                onAllLicenseLoaded: {
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
                                onAllDataProtectionLoaded: {
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

