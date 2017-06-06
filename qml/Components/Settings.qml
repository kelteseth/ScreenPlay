import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3


import "Settings/"

Page {
    pageName: ""


    VisualItemModel {
        id:settingsModel
        Headline {
            name: "General"
        }
        SettingBool {
            name: "Autostart"
            isChecked: settings.autostart
            //onCheckboxChanged: settings.setAutostart(checked)
        }
        Headline {
            name: "Misc"

        }
        SettingBool {
            name: "Autostart"
        }

    }



    ListView  {
        anchors.fill: parent
        anchors.margins: 20
        model:settingsModel
        boundsBehavior: Flickable.DragOverBounds
        cacheBuffer: 1000
        maximumFlickVelocity: 10000



    }
}
