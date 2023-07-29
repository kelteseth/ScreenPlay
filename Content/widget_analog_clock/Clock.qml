// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Item {
    id: clock
    width: {
        if (ListView.view && ListView.view.width >= 200)
            return ListView.view.width / Math.floor(ListView.view.width / 200.0);
        else
            return 200;
    }

    height: {
        if (ListView.view && ListView.view.height >= 240)
            return ListView.view.height;
        else
            return 240;
    }

    property alias city: cityLabel.text
    property int hours
    property int minutes
    property int seconds
    property real shift
    property bool night: false
    property bool internationalTime: true //Unset for local time

    function timeChanged() {
        var date = new Date;
        hours = internationalTime ? date.getUTCHours() + Math.floor(clock.shift) : date.getHours();
        night = (hours < 7 || hours > 19);
        minutes = internationalTime ? date.getUTCMinutes() + ((clock.shift % 1) * 60) : date.getMinutes();
        seconds = date.getUTCSeconds();
    }

    Timer {
        interval: 100
        running: true
        repeat: true
        onTriggered: clock.timeChanged()
    }

    Item {
        anchors.centerIn: parent
        width: 200
        height: 240

        Rectangle {
            width: 200
            height: width
            color: clock.night ? "black" : "gray"
            radius: width
        }

        Image {
            x: 92.5
            y: 27
            source: "hour.png"
            transform: Rotation {
                id: hourRotation
                origin.x: 7.5
                origin.y: 73
                angle: (clock.hours * 30) + (clock.minutes * 0.5)
                Behavior on angle  {
                    SpringAnimation {
                        spring: 2
                        damping: 0.2
                        modulus: 360
                    }
                }
            }
        }

        Image {
            x: 93.5
            y: 17
            source: "minute.png"
            transform: Rotation {
                id: minuteRotation
                origin.x: 6.5
                origin.y: 83
                angle: clock.minutes * 6
                Behavior on angle  {
                    SpringAnimation {
                        spring: 2
                        damping: 0.2
                        modulus: 360
                    }
                }
            }
        }

        Image {
            x: 97.5
            y: 20
            source: "second.png"
            transform: Rotation {
                id: secondRotation
                origin.x: 2.5
                origin.y: 80
                angle: clock.seconds * 6
                Behavior on angle  {
                    SpringAnimation {
                        spring: 2
                        damping: 0.2
                        modulus: 360
                    }
                }
            }
        }

        Image {
            anchors.centerIn: background
            source: "center.png"
        }

        Text {
            id: cityLabel
            y: 210
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            font.family: "Helvetica"
            font.bold: true
            font.pixelSize: 16
            style: Text.Raised
            styleColor: "black"
        }
    }
}
