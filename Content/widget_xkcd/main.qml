// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Particles

Item {
    id: root
    state: "normal"
    implicitWidth: defaultWidth
    implicitHeight: defaultHeight
    property int defaultWidth: 200
    property int defaultHeight: 200

    function request(url, callback) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = (function (myxhr) {
                return function () {
                    if (myxhr.readyState === 4)
                        callback(myxhr);
                };
            })(xhr);
        xhr.open('GET', url);
        xhr.send('');
    }

    Component.onCompleted: {
        request("http://xkcd.com/info.0.json", function (o) {
            if (o.status === 200) {
                var d = eval('new Object(' + o.responseText + ')');
                console.log(o.responseText);
                img.source = d.img;
            } else {
                console.log("Some error has occurred");
            }
        });
    }

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        property size imgSize: Qt.size(root.defaultWidth, defaultHeight)
        onStatusChanged: {
            if (img.status !== Image.Ready)
                return;
            if (img.sourceSize.width === 0 || img.sourceSize.height === 0)
                return;
            root.implicitWidth = img.sourceSize.width;
            root.implicitHeight = img.sourceSize.height;
            print(img.status, img.sourceSize.width, img.sourceSize.height);
            img.imgSize = Qt.size(img.sourceSize.width, img.sourceSize.height);
            print("img.size", img.imgSize);
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.state = root.state === "expanded" ? "normal" : "expanded";
            print(root.state, root.implicitHeight, root.implicitWidth);
        }
    }

    states: [
        State {
            PropertyChanges {
                name: "normal"
                root {
                    width: root.defaultWidth
                    height: root.defaultHeight
                    implicitWidth: root.defaultWidth
                    implicitHeight: root.defaultHeight
                }
            }
        },
        State {
            name: "expanded"
            PropertyChanges {
                root {
                    width: img.imgSize.width
                    height: img.imgSize.height
                    implicitWidth: img.imgSize.width
                    implicitHeight: img.imgSize.height
                }
            }
        }
    ]
}
