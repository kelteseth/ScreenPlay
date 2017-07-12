import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "Components"

ApplicationWindow {
    id: window
    color: "#eeeeee"
    visible: true
    width: 1380
    minimumHeight: 768
    minimumWidth: 1050


    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
    }

    Loader {
        id: tabLoader
        source : "qrc:/qml/Components/Installed.qml"
        anchors {
            top: nav.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }
        onStateChanged: {
            if(tabLoader.state === Loader.Loading){
                loaderText.visible = true
            } else if(tabLoader.state === Loader.Ready){
                loaderText.visible = false
            }
        }

        Text {
            id: loaderText
            visible: true
            text: qsTr("Loading...")
            anchors.centerIn: parent
            font.pixelSize: 32

            font.family: font_LibreBaskerville_Italic.name
            font.pointSize: 32
            font.italic: true
            color: "#818181"
            FontLoader{
                id: font_LibreBaskerville_Italic
                source: "qrc:/assets/fonts/LibreBaskerville-Italic.ttf"
            }
        }



//        onSourceChanged: tabLoaderAnim.running = true

//        SequentialAnimation {
//            id:tabLoaderAnim
//            running: true
//            NumberAnimation { target: tabLoader.item; property: "opacity"; from:0; to: 1; duration: 500 }
//            NumberAnimation { target: tabLoader.item; property: "y"; from: -100; to: 0; duration: 300 }
//        }

        Connections{
            target: tabLoader.item
            ignoreUnknownSignals: true
            onSetSidebaractiveItem: {
                if( sidebar.activeScreen == screenId && sidebar.state ==  "active"){
                    sidebar.state =  "inactive"
                } else {
                    sidebar.state =  "active"
                }

                sidebar.activeScreen = screenId

            }
        }
    }

    Sidebar {
        id: sidebar
        width:400
        anchors {
            top:nav.bottom
            right:parent.right
            bottom:parent.bottom
        }
    }

    Navigation {
        id: nav
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
        }
        onChangeTab: {
            tabLoader.setSource("qrc:/qml/Components/"+name+".qml")
            sidebar.state = "inactive"
        }

        onToggleMonitors: {
            monitors.state = monitors.state == "active" ? "inactive" : "active"
        }

    }

    Monitors {
        id: monitors
        state: "inactive"
        anchors.fill: tabLoader
        z:98
    }

    FileDropper {
        anchors.fill: parent
        z:99
    }

}
