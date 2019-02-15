import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id:monitorSelectionItem
    property rect monitorSize: Qt.rect(0, 0, 0, 0)
    property string monitorModel
    property string monitorManufacturer
    property string monitorName
    property string monitorID
    property string previewImage
    onPreviewImageChanged:  {
        imgPreview.source = previewImage
        imgPreview.opacity = 1
    }

    property int fontSize: 14
    property int index
    property bool isSelected: false
    property bool isWallpaperActive: false
    signal monitorSelected(var index)

    onIsSelectedChanged: {
        if (isSelected) {
            wrapper.border.color = "#F28E0D"

        } else {
            wrapper.border.color = "#373737"

        }
    }

    Text {
        
        text: monitorSize.width + "x" + monitorSize.height
        anchors{
            horizontalCenter: parent.horizontalCenter
            top:wrapper.bottom
            topMargin: 5
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        //color: "white"
        font.pixelSize: monitorSelectionItem.fontSize
        font.family: "Roboto"
        wrapMode: Text.WrapAnywhere
    }

    Rectangle {
        id: wrapper
        color: isSelected ? "#373737" : "#828282"
        anchors.fill: parent
        anchors.margins: 10
        border.color: "#1e1e1e"
        border.width: 3
        radius: 3
        clip:true

        Image {
            id: imgPreview
            sourceSize: Qt.size(parent.width,parent.height)
            anchors.margins: 3
            opacity: 0
            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectCrop

        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                monitorSelected(index)
            }
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
        }
    }
}
