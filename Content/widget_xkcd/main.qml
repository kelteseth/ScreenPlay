// SPDX-License-Identifier: BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import QtQuick.Particles

Item {
    id: root
    implicitWidth: 360
    implicitHeight: 360

    Image {
        id: img
        anchors.fill: parent
        onStatusChanged: {
            if (img.status !== Image.Ready)
                return;
            if (img.sourceSize.width === 0 || img.sourceSize.height === 0)
                return;
            root.implicitHeight = img.sourceSize.height;
            root.implicitWidth = img.sourceSize.width;
        }

        fillMode: Image.PreserveAspectCrop
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
}
