import QtQuick 2.14

Item {
    id: root

    property url source

    AnimatedImage {
        id: animatedImage
        source: root.source
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }
}
