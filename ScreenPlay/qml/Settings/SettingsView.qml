import QtQuick
import QtCore as QCore
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects
import ScreenPlay
import ScreenPlayCore

Item {
    id: root

    property Item modalSource

    Flickable {
        id: flickableWrapper

        width: 800
        height: parent.height
        contentHeight: columnWrapper.childrenRect.height
        contentWidth: 800
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {
            snapMode: ScrollBar.SnapOnRelease
        }

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

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    SettingBool {
                        headline: qsTr("Autostart")
                        description: qsTr("ScreenPlay will start with Windows and will setup your Desktop every time for you.")
                        isChecked: App.settings.autostart
                        onCheckboxChanged: function (checked) {
                            App.settings.setAutostart(checked);
                        }
                    }

                    // SettingsHorizontalSeperator {
                    // }

                    // SettingBool {
                    //     headline: qsTr("Show default installed content")
                    //     description: qsTr("ScreenPlay will show build in content.")
                    //     isChecked: App.settings.showDefaultContent
                    //     onCheckboxChanged: function (checked) {
                    //         App.settings.setShowDefaultContent(checked);
                    //         App.installedListModel.reset();
                    //     }
                    // }

                    SettingsHorizontalSeperator {}

                    SettingBool {
                        height: 70
                        headline: qsTr("Send anonymous crash reports and statistics")
                        description: qsTr("Help us make ScreenPlay faster and more stable. All collected data is purely anonymous and only used for development purposes! We use <a href=\"https://sentry.io\">sentry.io</a> to collect and analyze this data. A <b>big thanks to them</b> for providing us with free premium support for open source projects!")
                        isChecked: App.settings.anonymousTelemetry
                        onCheckboxChanged: function (checked) {
                            App.settings.setAnonymousTelemetry(checked);
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Set save location")
                        buttonText: qsTr("Set location")
                        description: {
                            // Remove file:/// so the used does not get confused
                            let path = App.globalVariables.localStoragePath + "";
                            if (path.length === 0)
                                return qsTr("Your storage path is empty!");
                            else
                                return path.replace('file:///', '');
                        }
                        onButtonPressed: {
                            folderDialogSaveLocation.open();
                        }

                        FolderDialog {
                            id: folderDialogSaveLocation
                            currentFolder: App.globalVariables.localStoragePath
                            onAccepted: {
                                App.settings.setLocalStoragePath(folderDialogSaveLocation.currentFolder);
                            }
                        }
                    }

                    Text {
                        id: txtDirChangesInfo

                        text: {
                            let text = qsTr("Important: Changing this directory has no effect on the workshop download path. ScreenPlay only supports having one content folder! Example Steam paths:\n");
                            if (App.globalVariables.isSteamVersion()) {
                                text += "\n    C:\\Program Files (x86)\\Steam\\steamapps\\workshop\\content\\672870\\";
                                text += "\n    F:\\SteamLibrary\\steamapps\\\workshop\\content\\672870\\";
                            }
                        }

                        color: Qt.darker(Material.foreground)
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        font.pointSize: 10
                        font.family: App.settings.font
                        height: App.globalVariables.isSteamVersion() ? 70 : 30

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
                        comboBox {
                            Component.onCompleted: comboBox.currentIndex = comboBox.indexOfValue(App.settings.language)
                            model: ListModel {
                                id: lmLangauge
                                ListElement {
                                    value: Settings.Language.En_US
                                    text: "English"
                                }
                                ListElement {
                                    value: Settings.Language.De_DE
                                    text: "German"
                                }
                                ListElement {
                                    value: Settings.Language.Pl_PL
                                    text: "Polish"
                                }
                                ListElement {
                                    value: Settings.Language.It_IT
                                    text: "Italian"
                                }
                                ListElement {
                                    value: Settings.Language.Zh_CN
                                    text: "Chinese - Simplified"
                                }
                                ListElement {
                                    value: Settings.Language.Ru_RU
                                    text: "Russian"
                                }
                                ListElement {
                                    value: Settings.Language.Fr_FR
                                    text: "French"
                                }
                                ListElement {
                                    value: Settings.Language.Es_ES
                                    text: "Spanish"
                                }
                                ListElement {
                                    value: Settings.Language.Ko_KR
                                    text: "Korean"
                                }
                                ListElement {
                                    value: Settings.Language.Vi_VN
                                    text: "Vietnamese"
                                }
                                ListElement {
                                    value: Settings.Language.Pt_BR
                                    text: "Portuguese (Brazil)"
                                }
                                ListElement {
                                    value: Settings.Language.Tr_TR
                                    text: "Turkish"
                                }
                                ListElement {
                                    value: Settings.Language.Nl_NL
                                    text: "Dutch"
                                }
                            }
                            onActivated: {
                                let language = settingsLanguage.comboBox.currentValue;
                                App.settings.setLanguage(language);
                                App.settings.retranslateUI();
                            }
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsComboBox {
                        id: settingsTheme

                        headline: qsTr("Theme")
                        description: qsTr("Switch dark/light theme")
                        comboBox {
                            Component.onCompleted: comboBox.currentIndex = comboBox.indexOfValue(App.settings.theme)
                            model: ListModel {
                                ListElement {
                                    value: Settings.Theme.System
                                    text: qsTr("System Default")
                                }
                                ListElement {
                                    value: Settings.Theme.Dark
                                    text: qsTr("Dark")
                                }
                                ListElement {
                                    value: Settings.Theme.Light
                                    text: qsTr("Light")
                                }
                            }
                            onActivated: {
                                App.settings.setTheme(settingsTheme.comboBox.currentValue);
                            }
                        }
                    }
                }
            }

            SettingsPage {

                header: SettingsHeader {
                    id: headerPerformance

                    text: qsTr("Wallpaper and Widgets")
                    image: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_build.svg"
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    SettingBool {
                        headline: qsTr("Pause wallpaper video rendering while another app is in the foreground.")
                        description: qsTr("Limitations: This setting is Windows only, currently only works if you have exactly one monitor connected, and is limited to wallpapers with no audio. Wallpaper restart is required, when changing this setting!")
                        isChecked: App.settings.checkWallpaperVisible
                        onCheckboxChanged: function (checked) {
                            App.settings.setCheckWallpaperVisible(checked);
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingBool {
                        headline: qsTr("Start Wallpaper Muted")
                        description: qsTr("Defaults to a muted wallpaper. You can always change this at a alter date in the Configure Content menu.")
                        isChecked: App.settings.startWallpaperMuted
                        onCheckboxChanged: function (checked) {
                            App.settings.setStartWallpaperMuted(checked);
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsComboBox {
                        id: cbVideoFillMode

                        headline: qsTr("Default Wallpaper Fill Mode")
                        description: qsTr("Set this property to define how the video is scaled to fit the target area.")
                        comboBox {
                            Component.onCompleted: comboBox.currentIndex = comboBox.indexOfValue(App.settings.videoFillMode)
                            model: ListModel {
                                ListElement {
                                    value: Video.FillMode.Stretch
                                    text: qsTr("Stretch")
                                }
                                ListElement {
                                    value: Video.FillMode.Fill
                                    text: qsTr("Fill")
                                }
                                ListElement {
                                    value: Video.FillMode.Contain
                                    text: qsTr("Contain")
                                }
                                ListElement {
                                    value: Video.FillMode.Cover
                                    text: qsTr("Cover")
                                }
                                ListElement {
                                    value: Video.FillMode.Scale_Down
                                    text: qsTr("Scale-Down")
                                }
                            }
                            onActivated: {
                                App.settings.setVideoFillMode(cbVideoFillMode.comboBox.currentValue);
                            }
                        }
                    }
                }
            }

            SettingsPage {

                header: SettingsHeader {
                    id: headerAbout

                    text: qsTr("About")
                    image: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_cake.svg"
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    Column {
                        id: settingsAboutrapperWrapper

                        width: parent.width
                        spacing: 10

                        Item {
                            width: parent.width
                            height: txtHeadline.paintedHeight + txtDescriptionAbout.paintedHeight + wrapperLinks.childrenRect.height + 80

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
                                    iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/brand_github.svg"
                                    url: "https://github.com/kelteseth"
                                    iconColor: "#eeeeee"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/brand_gitlab.svg"
                                    url: "https://gitlab.com/kelteseth"
                                    iconColor: "#FC6D26"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/brand_bluesky.svg"
                                    url: "https://bsky.app/profile/kelteseth.bsky.social"
                                    iconColor: "#1DA1F2"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/brand_twitch.svg"
                                    url: "https://www.twitch.tv/kelteseth/"
                                    color: "#6441A5"
                                }

                                GrowIconLink {
                                    iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/brand_reddit.svg"
                                    url: "https://www.reddit.com/r/ScreenPlayApp/"
                                    iconColor: "#FF4500"
                                }
                            }

                            Image {
                                id: imgLogoHead

                                source: "https://gitlab.com/uploads/-/system/user/avatar/64172/avatar.png"
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

                                source: "qrc:/qt/qml/ScreenPlay/assets/images/mask_round.svg"
                                sourceSize: Qt.size(width, height)
                                smooth: true
                                width: 120
                                height: 120
                                visible: false
                            }

                            MultiEffect {
                                id: opacityMask
                                anchors.fill: imgLogoHead
                                source: imgLogoHead
                                maskEnabled: true
                                maskSource: mask
                            }
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        icon.source: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_launch.svg"
                        headline: qsTr("Version")
                        description: App.settings.buildInfos
                        buttonText: qsTr("Open Changelog")
                        onButtonPressed: Qt.openUrlExternally("https://gitlab.com/kelteseth/ScreenPlay/-/releases")
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Third Party Software")
                        description: qsTr("ScreenPlay would not be possible without the work of others. A big thank you to: ")
                        buttonText: qsTr("Licenses")
                        onButtonPressed: {
                            App.util.requestAllLicenses();
                            expanderCopyright.toggle();
                        }
                    }

                    SettingsExpander {
                        id: expanderCopyright

                        Connections {
                            function onAllLicenseLoaded(licensesText) {
                                expanderCopyright.text = licensesText;
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
                            const logsPath = QCore.StandardPaths.writableLocation(QCore.StandardPaths.TempLocation) + "/ScreenPlay/Logs";
                            App.util.openFolderInExplorer(logsPath);
                        }
                    }

                    SettingsHorizontalSeperator {}

                    SettingsButton {
                        headline: qsTr("Data Protection")
                        description: qsTr("We use you data very carefully to improve ScreenPlay. We do not sell or share this (anonymous) information with others!")
                        buttonText: qsTr("Privacy")
                        onButtonPressed: {
                            App.util.requestDataProtection();
                            expanderDataProtection.toggle();
                        }
                    }

                    SettingsExpander {
                        id: expanderDataProtection

                        Connections {
                            function onAllDataProtectionLoaded(dataProtectionText) {
                                expanderDataProtection.text = dataProtectionText;
                            }

                            target: App.util
                        }
                    }
                }
            }
        }
    }
}
