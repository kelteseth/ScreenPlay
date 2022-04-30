import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import ScreenPlayApp 1.0
import ScreenPlay 1.0
import ScreenPlay.Enums.FillMode 1.0
import Settings 1.0
import ScreenPlayUtil 1.0 

Item {
    id: root

    property Item modalSource

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

        Column {
            id: columnWrapper

            width: parent.width - 40
            spacing: 30

            SettingsPage {
                id: settingsGeneralWrapper

                header: SettingsHeader {
                    id: headerGeneral

                    text: qsTr("General")
                }

                contentItem: Column {
                    id: columnGeneral

                    spacing: 20

                    anchors {
                        top: headerGeneral.bottom
                        topMargin: 20
                        right: parent.right
                        left: parent.left
                        leftMargin: 20
                        rightMargin: 20
                    }

                    SettingBool {
                        headline: qsTr("Autostart")
                        description: qsTr("ScreenPlay will start with Windows and will setup your Desktop every time for you.")
                        isChecked: App.settings.autostart
                        onCheckboxChanged: function (checked) {
                            App.settings.setAutostart(checked)
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingBool {
                        headline: qsTr("High priority Autostart")
                        available: false
                        description: qsTr("This options grants ScreenPlay a higher autostart priority than other apps.")
                        isChecked: App.settings.highPriorityStart
                        onCheckboxChanged: {
                            App.settings.setHighPriorityStart(checked)
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingBool {
                        height: 70
                        headline: qsTr("Send anonymous crash reports and statistics")
                        description: qsTr("Help us make ScreenPlay faster and more stable. All collected data is purely anonymous and only used for development purposes! We use <a href=\"https://sentry.io\">sentry.io</a> to collect and analyze this data. A <b>big thanks to them</b> for providing us with free premium support for open source projects!")
                        isChecked: App.settings.anonymousTelemetry
                        onCheckboxChanged: function (checked) {
                            App.settings.setAnonymousTelemetry(checked)
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Set save location")
                        buttonText: qsTr("Set location")
                        description: {
                            // Remove file:/// so the used does not get confused
                            let path = App.globalVariables.localStoragePath + ""
                            if (path.length === 0)
                                return qsTr("Your storage path is empty!")
                            else
                                return path.replace('file:///', '')
                        }
                        onButtonPressed: {
                            folderDialogSaveLocation.open()
                        }

                        FolderDialog {
                            id: folderDialogSaveLocation
                            folder: App.globalVariables.localStoragePath
                            onAccepted: {
                                App.settings.setLocalStoragePath(
                                            folderDialogSaveLocation.currentFolder)
                            }
                        }
                    }

                    Text {
                        id: txtDirChangesInfo

                        text: qsTr("Important: Changing this directory has no effect on the workshop download path. ScreenPlay only supports having one content folder!")
                        color: Qt.darker(Material.foreground)
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        font.pointSize: 10
                        font.family: App.settings.font
                        height: 30

                        anchors {
                            right: parent.right
                            rightMargin: 10
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
                                        App.settings.language)
                        }

                        comboBox {
                            model: [{
                                    "value": Settings.En_US,
                                    "text": "English"
                                }, {
                                    "value": Settings.De_DE,
                                    "text": "German"
                                }, {
                                    "value": Settings.Pl_PL,
                                    "text": "Polish"
                                }, {
                                    "value": Settings.It_IT,
                                    "text": "Italian"
                                }, {
                                    "value": Settings.Zh_CN,
                                    "text": "Chinese - Simplified"
                                }, {
                                    "value": Settings.Ru_RU,
                                    "text": "Russian"
                                }, {
                                    "value": Settings.Fr_FR,
                                    "text": "French"
                                }, {
                                    "value": Settings.Es_ES,
                                    "text": "Spanish"
                                }, {
                                    "value": Settings.Ko_KR,
                                    "text": "Korean"
                                }, {
                                    "value": Settings.Vi_VN,
                                    "text": "Vietnamese"
                                }, {
                                    "value": Settings.Pt_BR,
                                    "text": "Portuguese (Brazil)"
                                }, {
                                    "value": Settings.Tr_TR,
                                    "text": "Turkish"
                                }, {
                                    "value": Settings.Nl_NL,
                                    "text": "Dutch"
                                }]
                            onActivated: {
                                App.settings.setLanguage(
                                            settingsLanguage.comboBox.currentValue)
                                App.settings.retranslateUI()
                            }
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsComboBox {
                        id: settingsTheme

                        headline: qsTr("Theme")
                        description: qsTr("Switch dark/light theme")
                        Component.onCompleted: {
                            settingsTheme.comboBox.currentIndex = root.indexOfValue(
                                        settingsTheme.comboBox.model,
                                        App.settings.theme)
                        }

                        comboBox {
                            model: [{
                                    "value": Settings.System,
                                    "text": qsTr("System Default")
                                }, {
                                    "value": Settings.Dark,
                                    "text": qsTr("Dark")
                                }, {
                                    "value": Settings.Light,
                                    "text": qsTr("Light")
                                }]
                            onActivated: {
                                App.settings.setTheme(
                                            settingsTheme.comboBox.currentValue)
                            }
                        }
                    }
                }
            }

            SettingsPage {

                header: SettingsHeader {
                    id: headerPerformance

                    text: qsTr("Performance")
                    image: "qrc:/qml/ScreenPlayApp/assets/icons/icon_build.svg"
                }

                contentItem: Column {
                    id: perfomanceWrapper

                    spacing: 20

                    anchors {
                        top: headerPerformance.bottom
                        topMargin: 20
                        right: parent.right
                        left: parent.left
                        leftMargin: 20
                        rightMargin: 20
                    }

                    SettingBool {
                        headline: qsTr("Pause wallpaper video rendering while another app is in the foreground")
                        description: qsTr("We disable the video rendering (not the audio!) for the best performance. If you have problem you can disable this behaviour here. Wallpaper restart required!")
                        isChecked: App.settings.checkWallpaperVisible
                        onCheckboxChanged: function (checked) {
                            App.settings.setCheckWallpaperVisible(
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
                                        App.settings.videoFillMode)
                        }

                        comboBox {
                            onActivated: App.settings.setVideoFillMode(
                                             cbVideoFillMode.comboBox.currentValue)
                            model: [{
                                    "value": FillMode.Stretch,
                                    "text": qsTr("Stretch")
                                }, {
                                    "value": FillMode.Fill,
                                    "text": qsTr("Fill")
                                }, {
                                    "value": FillMode.Contain,
                                    "text": qsTr("Contain")
                                }, {
                                    "value": FillMode.Cover,
                                    "text": qsTr("Cover")
                                }, {
                                    "value": FillMode.Scale_Down,
                                    "text": qsTr("Scale-Down")
                                }]
                        }
                    }
                }
            }

            SettingsPage {

                header: SettingsHeader {
                    id: headerAbout

                    text: qsTr("About")
                    image: "qrc:/qml/ScreenPlayApp/assets/icons/icon_cake.svg"
                }

                contentItem: Column {
                    id: aboutWrapper

                    spacing: 20

                    anchors {
                        top: headerAbout.bottom
                        topMargin: 20
                        right: parent.right
                        left: parent.left
                        leftMargin: 20
                        rightMargin: 20
                    }

                    Column {
                        id: settingsAboutrapperWrapper

                        width: parent.width
                        spacing: 10

                        Item {
                            width: parent.width
                            height: txtHeadline.paintedHeight + txtDescriptionAbout.paintedHeight
                                    + wrapperLinks.childrenRect.height + 80

                            Text {
                                id: txtHeadline

                                color: Material.foreground
                                text: qsTr("Thank you for using ScreenPlay")
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                font.pointSize: 16
                                font.family: App.settings.font

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
                                color: Qt.darker(Material.foreground)
                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignTop
                                horizontalAlignment: Text.AlignLeft
                                font.pointSize: 11
                                font.family: App.settings.font
                                width: parent.width * 0.6

                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: 15
                                    left: parent.left
                                    leftMargin: 20
                                    right: imgLogoHead.left
                                    rightMargin: 60
                                }
                            }

                            RowLayout {
                                id: wrapperLinks

                                spacing: 20

                                anchors {
                                    left: parent.left
                                    margins: 20
                                    bottom: parent.bottom
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/brand_github.svg"
                                    url: "https://github.com/kelteseth"
                                    color: "#333333"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/brand_gitlab.svg"
                                    url: "https://gitlab.com/kelteseth"
                                    color: "#FC6D26"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/brand_twitter.svg"
                                    url: "https://twitter.com/Kelteseth"
                                    color: "#1DA1F2"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/brand_twitch.svg"
                                    url: "https://www.twitch.tv/kelteseth/"
                                    color: "#6441A5"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qml/ScreenPlayApp/assets/icons/brand_reddit.svg"
                                    url: "https://www.reddit.com/r/ScreenPlayApp/"
                                    color: "#FF4500"
                                }
                            }

                            Image {
                                id: imgLogoHead

                                source: "https://assets.gitlab-static.net/uploads/-/system/user/avatar/64172/avatar.png"
                                width: 120
                                height: 120
                                visible: false
                                sourceSize: Qt.size(width, height)

                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: 0
                                    right: parent.right
                                    rightMargin: 20
                                }
                            }

                            Image {
                                id: mask

                                source: "qrc:/qml/ScreenPlayApp/assets/images/mask_round.svg"
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
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        icon.source: "qrc:/qml/ScreenPlayApp/assets/icons/icon_launch.svg"
                        headline: qsTr("Version")
                        description: qsTr("ScreenPlay Build Version \n")
                                     + App.settings.gitBuildHash
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
                            App.util.requestAllLicenses()
                            expanderCopyright.toggle()
                        }
                    }

                    SettingsExpander {
                        id: expanderCopyright

                        Connections {
                            function onAllLicenseLoaded(licensesText) {
                                expanderCopyright.text = licensesText
                            }

                            target: App.util
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Logs")
                        description: qsTr("If your ScreenPlay missbehaves this is a good way to look for answers. This shows all logs and warning during runtime.")
                        buttonText: qsTr("Show Logs")
                        onButtonPressed: {
                            expanderDebug.toggle()
                        }
                    }

                    SettingsExpander {
                        id: expanderDebug

                        text: App.util.debugMessages
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Data Protection")
                        description: qsTr("We use you data very carefully to improve ScreenPlay. We do not sell or share this (anonymous) information with others!")
                        buttonText: qsTr("Privacy")
                        onButtonPressed: {
                            App.util.requestDataProtection()
                            expanderDataProtection.toggle()
                        }
                    }

                    SettingsExpander {
                        id: expanderDataProtection

                        Connections {
                            function onAllDataProtectionLoaded(dataProtectionText) {
                                expanderDataProtection.text = dataProtectionText
                            }

                            target: App.util
                        }
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }
    }
}
