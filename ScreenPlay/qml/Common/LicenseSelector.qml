import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12

import ScreenPlay 1.0

ColumnLayout {
    id: root

    property alias comboBox: cb

    HeadlineSection {
        Layout.fillWidth: true
        text: qsTr("License")
    }

    ComboBox {
        id: cb
        Layout.fillWidth: true
        font.family: ScreenPlay.settings.font
        model: ListModel {
            id: modelLicense
            ListElement {
                text: "Open Source - General Public License 3.0"
                description: qstr("You grant other to remix your work but it must remain under the GPLv3. We recommend this license for all code wallpaper!")
                tldrlegal: "https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)"
                licenseFile: ":/assets/wizards/License_GPL_3.0.txt"
            }
            ListElement {
                text: "Open Source - Apache License 2.0"
                description: qstr("You grant other to remix your work and change the license to their linking.")
                tldrlegal: "https://tldrlegal.com/license/apache-license-2.0-(apache-2.0)"
                licenseFile: ":/assets/wizards/License_Apache_2.0.txt"
            }

            ListElement {
                text: "Create Commons - Attribution 4.0"
                description: qstr("You grant other to remix your work and change the license to their linking.")
                tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-4.0-international-(cc-by-4)"
                licenseFile: ":/assets/wizards/License_CC_Attribution_4.0.txt"
            }
            ListElement {
                text: "Create Commons - Attribution-ShareAlike 4.0"
                description: qstr("Share — copy and redistribute the material in any medium or format. Adapt — remix, transform, and build upon the material.
for any purpose, even commercially.")
                tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-sharealike-4.0-international-(cc-by-sa-4.0)"
                licenseFile: ":/assets/wizards/License_CC_Attribution-ShareAlike_4.0.txt"
            }
            ListElement {
                text: "Create Commons - Attribution-NonCommercial-ShareAlike 4.0"
                description: qstr("Share — copy and redistribute the material in any medium or format. Adapt — remix, transform, and build upon the material. You are not allowed to use it commercially! ")
                tldrlegal: "https://tldrlegal.com/license/creative-commons-attribution-noncommercial-sharealike-4.0-international-(cc-by-nc-sa-4.0)"
                licenseFile: ":/assets/wizards/License_CC_Attribution-NonCommercial-ShareAlike_4.0.txt"
            }
            ListElement {
                text: "Create Commons - CC0 1.0 Universal Public Domain"
                description: "You allow everyone do do anything with your work."
                tldrlegal: "https://tldrlegal.com/license/creative-commons-cc0-1.0-universal"
                licenseFile: ":/assets/wizards/License_CC0_1.0.txt"
            }
            ListElement {
                text: "Copyright by me"
                description: "You do not share any rights and nobody is allowed to use remix it (Not recommended)."
                tldrlegal: ""
            }
        }
    }
}
