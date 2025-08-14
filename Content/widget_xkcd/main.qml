// SPDX-License-Identifier: BSD-3-Clause
import QtQuick

Item {
    id: root
    implicitWidth: 300
    implicitHeight: 200

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
                img.source = d.img;
            } else {
                console.log("Some error has occurred");
            }
        });
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: img.width
        contentHeight: img.height
        interactive: true
        clip: true

        Image {
            id: img
            width: Math.min(sourceSize.width, root.width)
            height: Math.min(sourceSize.height, root.height)
            fillMode: Image.PreserveAspectFit
        }

        MouseArea {
            anchors.fill: img
            onDoubleClicked: {
                if (flickable.contentWidth > flickable.width || flickable.contentHeight > flickable.height) {
                    // Zoom out to fit
                    img.width = Math.min(img.sourceSize.width, root.width);
                    img.height = Math.min(img.sourceSize.height, root.height);
                } else {
                    // Zoom in to actual size
                    img.width = img.sourceSize.width;
                    img.height = img.sourceSize.height;
                }
            }

            onWheel: function (wheel) {
                var scaleFactor = wheel.angleDelta.y > 0 ? 1.1 : 0.9;
                var newWidth = img.width * scaleFactor;
                var newHeight = img.height * scaleFactor;

                // Limit zoom range
                var minScale = Math.min(root.width / img.sourceSize.width, root.height / img.sourceSize.height);
                var maxScale = 3.0;

                if (newWidth >= img.sourceSize.width * minScale && newWidth <= img.sourceSize.width * maxScale) {
                    img.width = newWidth;
                    img.height = newHeight;
                }
            }
        }
    }
}
