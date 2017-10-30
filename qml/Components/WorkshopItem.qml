import QtQuick 2.7

Rectangle {
    id:workshopItem
    color: "steelblue"

    width: 188
    height: 106
    clip:true
    radius: 4

    property url imgUrl
    property string name

    Image {
        id: img
        anchors.fill: parent
        asynchronous: true
        clip:true
        cache: true
        source: workshopItem.imgUrl
        smooth: false
    }
    Text {
        id: namea
        anchors.centerIn: parent
        text: workshopItem.name
        color: "white"
    }
}
