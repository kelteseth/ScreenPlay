import QtQuick 2.0
import QtQuick.Controls 2.0


Rectangle {
    id:navigation
    height:60
    clip: true
    width: 1366

    signal changeTab(string name)
    signal toggleMonitors()

    function onTabChanged(name){
         navigation.changeTab(name);
        if(name ==="Workshop")
        {
            navWorkshop.state = "active"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"
            navCreate.state = "inactive"

        } else if(name ==="Installed")
        {
            navWorkshop.state = "inactive"
            navInstalled.state = "active"
            navSettings.state = "inactive"
            navCreate.state = "inactive"

        } else if(name ==="Settings")
        {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "active"
            navCreate.state = "inactive"

        } else if(name ==="Create")
        {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"
            navCreate.state = "active"
        }
    }

    Row {
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: 20
        spacing: 20

        NavigationItem {
            id: navCreate
            state: "inactive"
            name: "Create"
            iconSource: "qrc:/assets/icons/icon_plus.svg"
            onTabClicked: navigation.onTabChanged(name)
        }

        NavigationItem {
            id: navWorkshop
            state: "inactive"
            name: "Workshop"
            iconSource: "qrc:/assets/icons/icon_workshop.svg"
            onTabClicked: navigation.onTabChanged(name)
        }

        NavigationItem {
            id: navInstalled
            state: "active"
            name: "Installed"
            iconSource: "qrc:/assets/icons/icon_installed.svg"
            onTabClicked: navigation.onTabChanged(name)

        }

        NavigationItem {
            id: navSettings
            state: "inactive"
            name: "Settings"
            iconSource: "qrc:/assets/icons/icon_settings.svg"
            onTabClicked: navigation.onTabChanged(name)

        }
    }

    Item {
        id: monitorSelection
        width: 321
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                toggleMonitors()
            }

        }


        Image {
            id: image
            width: 24
            height: 24
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            sourceSize.height: 24
            sourceSize.width: 24
            source: "qrc:/assets/icons/icon_monitor.svg"
        }

        Text {
            id: activeMonitorName
            text: qsTr("Monitor Setup")
            anchors.right: image.right
            anchors.rightMargin: 20 + image.width
            horizontalAlignment: Text.AlignRight
            color: "#626262"
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 16
            font.family: font_Roboto_Regular.name
            renderType: Text.NativeRendering

            FontLoader{
                id: font_Roboto_Regular
                source: "qrc:/assets/fonts/Roboto-Regular.ttf"
            }
        }

    }


}
