import QtQuick 2.7

Item {
    anchors.fill: parent

    property url file
    onFileChanged: print(file)
}
