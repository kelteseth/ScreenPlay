import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "Settings/"

CustomPage {
    id: settings
    pageName: ""

    VisualItemModel {
        id: settingsModel


        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30

                anchors.fill: parent
                Headline {
                    name: "General"
                    height: 50
                    width: parent.width
                }
                SettingBool {
                    name: "Autostart"
                    height: 50
                    width: parent.width

                    //onCheckboxChanged: settings.setAutostart(checked)
                }
            }
        }
        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30

                anchors.fill: parent
                Headline {
                    name: "General"
                    height: 50
                    width: parent.width
                }
                SettingBool {
                    name: "Autostart"
                    height: 50
                    width: parent.width

                    //onCheckboxChanged: settings.setAutostart(checked)
                }
            }
        }
        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30

                anchors.fill: parent
                Headline {
                    name: "General"
                    height: 50
                    width: parent.width
                }
                SettingBool {
                    name: "Autostart"
                    height: 50
                    width: parent.width

                    //onCheckboxChanged: settings.setAutostart(checked)
                }
            }
        }
        SettingsWrapper {
            height: 180
            Column {
                spacing: 10
                anchors.margins: 30

                anchors.fill: parent
                Headline {
                    name: "General"
                    height: 50
                    width: parent.width
                }
                SettingBool {
                    name: "Autostart"
                    height: 50
                    width: parent.width

                    //onCheckboxChanged: settings.setAutostart(checked)
                }
            }
        }
    }

    ListView {
        id: settingsListView
        anchors {
            fill: parent
            topMargin: 30
            bottomMargin: 30
            rightMargin: 15
            leftMargin: 15
        }
        spacing: 30
        model: settingsModel

        cacheBuffer: 100
        maximumFlickVelocity: 3000
        ScrollBar.vertical: ScrollBar {
        }
    }
}
