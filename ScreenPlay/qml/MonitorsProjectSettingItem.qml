import QtQuick 2.9
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

Item {
    id: delegate
    focus: true
    height: isHeadline ? 50 : 30
    width: 300


    property int selectedMonitor

    anchors {
        left: parent.left
        leftMargin: isHeadline ? 0 : 25
    }
    
    Text {
        id: txtDescription
        text: name
        width: 100
        font.pixelSize: isHeadline ? 21 : 14
        anchors.verticalCenter: parent.verticalCenter
        font.family: "Roboto"
        font.weight: Font.Normal
        renderType: Text.NativeRendering
        anchors {
            left: parent.left
        }
    }
    
    Item {
        height: parent.height
        visible: !isHeadline
        anchors {
            left: txtDescription.right
            leftMargin: 20
            right: parent.right
        }
        
        TextEdit {
            id:teValue
            anchors.fill: parent
            anchors.margins: 4
            focus: true
            font.family: "Roboto"
            font.weight: Font.Normal
            renderType: Text.NativeRendering
            // We need to convert to a string because of large numbers js uses scientific notation x*e^x
            text: value.toString()
            Keys.onReturnPressed: screenPlay.setWallpaperValue(selectedMonitor,txtDescription.text,teValue.text)
            
        }
    }
}
