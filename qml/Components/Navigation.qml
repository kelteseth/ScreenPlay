import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle {
    id:navigation
    height:60
    width: 1366

    signal changeTab(string name)

    function onTabChanged(name){
         navigation.changeTab(name);
        if(name ==="Workshop")
        {
            navWorkshop.state = "active"
            navInstalled.state = "inactive"
            navSettings.state = "inactive"

        } else if(name ==="Installed")
        {
            navWorkshop.state = "inactive"
            navInstalled.state = "active"
            navSettings.state = "inactive"

        } else if(name ==="Settings")
        {
            navWorkshop.state = "inactive"
            navInstalled.state = "inactive"
            navSettings.state = "active"
        }
    }

    Row {
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: 20
        spacing: 20

        NavigationItem {
            id: navWorkshop
            state: "active"
            name: "Workshop"
            iconSource: "qrc:/assets/icons/icon_workshop.svg"
            onTabClicked: navigation.onTabChanged(name)
        }

        NavigationItem {
            id: navInstalled
            state: "inactive"
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
        id:createScreenPlay
        width: txtcreateScreenPlay.contentWidth

        anchors{
            top:parent.top
            right: parent.right
            rightMargin: 20
            bottom: parent.bottom
        }

        MouseArea {
            anchors.fill: parent

            Text {
                id: txtcreateScreenPlay
                text: qsTr("+ Create ScreenPlay")
                anchors.centerIn: parent
            }
        }

    }

}
