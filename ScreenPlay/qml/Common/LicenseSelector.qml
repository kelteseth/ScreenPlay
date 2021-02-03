import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import ScreenPlay 1.0

ColumnLayout {
    id: root
    Layout.preferredWidth: 250

    property string name: licenseModel.get(cb.currentIndex).name
    property string licenseFile: licenseModel.get(cb.currentIndex).licenseFile

    HeadlineSection {
        Layout.fillWidth: true
        text: qsTr("License")
    }

    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 5

        ComboBox {
            id: cb
            Layout.fillWidth: true
            textRole: "name"
            model: ListModel {
                id: licenseModel
                ListElement {
                    name: "Creative Commons - Attribution-ShareAlike 4.0"
                    description: qsTr("Share — copy and redistribute the material in any medium or format. Adapt — remix, transform, and build upon the material for any purpose, even commercially.")
                    tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-sharealike-4.0-international-(cc-by-sa-4.0)"
                    licenseFile: "License_CC_Attribution-ShareAlike_4.0.txt"
                }
                ListElement {
                    name: "Creative Commons - Attribution 4.0"
                    description: qsTr("You grant other to remix your work and change the license to their linking.")
                    tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-4.0-international-(cc-by-4)"
                    licenseFile: "License_CC_Attribution_4.0.txt"
                }
                ListElement {
                    name: "Creative Commons - Attribution-NonCommercial-ShareAlike 4.0"
                    description: qsTr("Share — copy and redistribute the material in any medium or format. Adapt — remix, transform, and build upon the material. You are not allowed to use it commercially! ")
                    tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-noncommercial-sharealike-4.0-international-(cc-by-nc-sa-4.0)"
                    licenseFile: "License_CC_Attribution-NonCommercial-ShareAlike_4.0.txt"
                }
                ListElement {
                    name: "Creative Commons - CC0 1.0 Universal Public Domain"
                    description: qsTr("You allow everyone to do anything with your work.")
                    tldrlegal: "https://tldrlegal.com/license/creative-commons-cc0-1.0-universal"
                    licenseFile: "License_CC0_1.0.txt"
                }
                ListElement {
                    name: "Open Source - Apache License 2.0"
                    description: qsTr("You grant other to remix your work and change the license to their linking.")
                    tldrlegal: "https://tldrlegal.com/license/apache-license-2.0-(apache-2.0)"
                    licenseFile: "License_Apache_2.0.txt"
                }
                ListElement {
                    name: "Open Source - General Public License 3.0"
                    description: qsTr("You grant other to remix your work but it must remain under the GPLv3. We recommend this license for all code wallpaper!")
                    tldrlegal: "https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)"
                    licenseFile: "License_GPL_3.0.txt"
                }
                ListElement {
                    name: "All rights reserved"
                    description: qsTr("You do not share any rights and nobody is allowed to use or remix it (Not recommended). Can also used to credit work others.")
                    tldrlegal: "License_All_Rights_Reserved_1.0.txt"
                }
            }
        }

        ToolButton {
            icon.source: "qrc:/assets/icons/icon_help_center.svg"
            icon.color: Material.iconColor
            onClicked: toolTip.open()
        }

        ToolButton {
            icon.source: "qrc:/assets/icons/icon_open_in_new.svg"
            icon.color: Material.iconColor
            onClicked: Qt.openUrlExternally(licenseModel.get(
                                                cb.currentIndex).tldrlegal)
        }

        ToolTip {
            id: toolTip
            text: licenseModel.get(cb.currentIndex).description
        }
    }
}
