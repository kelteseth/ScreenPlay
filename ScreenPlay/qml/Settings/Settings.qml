import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0

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
        maximumFlickVelocity: 5000
        flickDeceleration: 5000

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
                            isChecked: screenPlaySettings.autostart
                            onCheckboxChanged: {
                                screenPlaySettings.setAutostart(checked)
                                screenPlaySettings.writeSingleSettingConfig(
                                            "autostart", checked)
                            }
                        }
                        SettingsHorizontalSeperator {
                        }
                        SettingBool {
                            headline: qsTr("High priority Autostart")
                            available: false

                            description: qsTr("This options grants ScreenPlay a higher autostart priority than other apps.")
                            isChecked: screenPlaySettings.highPriorityStart
                            onCheckboxChanged: {
                                screenPlaySettings.setHighPriorityStart(checked)
                                screenPlaySettings.writeSingleSettingConfig(
                                            "highPriorityStart", checked)
                            }
                        }
                        SettingsHorizontalSeperator {
                        }
                        SettingBool {
                            height: 70
                            available: false
                            headline: qsTr("Send anonymous crash reports and statistics")
                            description: qsTr("Help us make ScreenPlay faster and more stable. All collected data is purely anonymous and only used for development purposes!")
                            isChecked: screenPlaySettings.sendStatistics
                            onCheckboxChanged: {
                                screenPlaySettings.setSendStatistics(checked)
                                screenPlaySettings.writeSingleSettingConfig(
                                            "sendStatistics", checked)
                            }

                        }
                        SettingsHorizontalSeperator {
                        }

                        SettingsButton {
                            headline: qsTr("Set save location")
                            description: globalVariables.localStoragePath //qsTr("Choose where to find you content. The default save location is you steam installation")
                            buttonText: qsTr("Set location")
                            onButtonPressed: {
                                folderDialogSaveLocation.open()
                            }
                            FolderDialog {
                                id: folderDialogSaveLocation
                                onAccepted: {
                                    globalVariables.setLocalStoragePath(
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
                            anchors{
                                right:parent.right
                                left:parent.left
                                leftMargin: 20
                            }
                        }

                        SettingsHorizontalSeperator {
                        }
                        SettingsComboBox {
                            id: settingsLanguage
                            headline: qsTr("Language")
                            description: qsTr("Set the ScreenPlay UI Language")
                            onCurrentIndexChanged: {
                                var key = settingsLanguage.comboBoxListModel.get(
                                            settingsLanguage.currentIndex).text.toString();

                                var languageKey;

                                switch (key) {
                                case "German":
                                    languageKey = "de"
                                    break;
                                case "English":
                                    languageKey = "en"
                                    break;
                                case "Russian":
                                    languageKey = "ru"
                                    break;
                                default:
                                    languageKey = "en"
                                    break;
                                }
                                print(key, languageKey)

                                screenPlaySettings.setqSetting("language", languageKey)

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
                            isChecked: screenPlaySettings.pauseWallpaperWhenIngame
                            onCheckboxChanged: {
                                screenPlaySettings.setPauseWallpaperWhenIngame(checked)
                                screenPlaySettings.writeSingleSettingConfig("setPauseWallpaperWhenIngame",checked)
                            }
                        }
                        SettingsHorizontalSeperator {
                        }
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
                        id:settingsAboutrapperWrapper
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
                                text: "Thank you for using ScreenPlay"

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
                                text: "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. \n \n Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. "
                                color: "#B5B5B5"

                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignTop
                                horizontalAlignment: Text.AlignLeft
                                font.pointSize: 10
                                font.family: "Roboto"
                                width: parent.width * .6
                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: 6
                                    left: parent.left
                                    leftMargin: 20
                                    right: imgLogoHead.left
                                    rightMargin: 20
                                    bottom: parent.bottom
                                }
                            }
                            Image {
                                id: imgLogoHead
                                source: "qrc:/assets/icons/icon_logo_head.svg"
                                width: 150
                                height: 150
                                sourceSize: Qt.size(150, 150)
                                anchors {
                                    top: txtHeadline.bottom
                                    topMargin: -10
                                    right: parent.right
                                    rightMargin: 20
                                }
                            }
                        }
                        SettingsHorizontalSeperator {}
                        Item {
                            width: parent.width
                            height: 55

                            Text {
                                id:txtVersion
                                color: "#5D5D5D"
                                text: "Version"

                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                anchors{
                                    top:parent.top
                                    topMargin: 6
                                    left:parent.left
                                    leftMargin: 20

                                }

                               font.pointSize: 12
                            }
                            Text {
                                text: qsTr("ScreenPlay Build Version ") + screenPlaySettings.gitBuildHash
                                color: "#B5B5B5"

                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignVCenter

                                font.pointSize: 10
                                font.family: "Roboto"
                                anchors {
                                    top:txtVersion.bottom
                                    topMargin: 6
                                    left:parent.left
                                    leftMargin: 20
                                }
                            }
                        }
                        SettingsHorizontalSeperator {}
                        SettingsButton {
                            headline: qsTr("Third Party Software")
                            description: qsTr("ScreenPlay would not be possible without the work of others. A big thank you to: ")
                            buttonText: qsTr("Licenses")
                            onButtonPressed: {
                                utility.requestAllLicenses()
                                expanderCopyright.toggle()
                            }
                        }
                        SettingsExpander {
                            id:expanderCopyright
                            anchors {
                                left: parent.left
                                right: parent.right
                            }


                            Connections {
                                target: utility
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
                            id:expanderDebug
                            text: utility.debugMessages
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
                                utility.requestAllLDataProtection()
                                expanderDataProtection.toggle()
                            }
                        }
                        SettingsExpander {
                            id:expanderDataProtection
                            anchors {
                                left: parent.left
                                right: parent.right
                            }


                            Connections {
                                target: utility
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

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
