/*
    Based on: https://github.com/albertino80/bppgrid

    MIT License

    Copyright (c) 2019 Alberto Bignotti

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Shapes 1.12

Pane {
    property color hueColor: "blue"
    property int colorHandleRadius: 10
    property int marginsValue: 10
    property int chekerSide: 5
    property color currentColor: "black"
    property var commonColors: ['#FFFFFF', '#C0C0C0', '#808080', '#000000', '#FF0000', '#800000', '#FFFF00', '#808000', '#00FF00', '#008000', '#00FFFF', '#008080', '#0000FF', '#000080', '#FF00FF', '#800080']
    property bool updatingControls: false

    function initColor(acolor) {
        initColorRGB(acolor.r * 255, acolor.g * 255, acolor.b * 255, acolor.a * 255);
    }

    function setHueColor(hueValue) {
        var v = 1 - hueValue;
        if (0 <= v && v < 0.16) {
            return Qt.rgba(1, 0, v / 0.16, 1);
        } else if (0.16 <= v && v < 0.33) {
            return Qt.rgba(1 - (v - 0.16) / 0.17, 0, 1, 1);
        } else if (0.33 <= v && v < 0.5) {
            return Qt.rgba(0, ((v - 0.33) / 0.17), 1, 1);
        } else if (0.5 <= v && v < 0.76) {
            return Qt.rgba(0, 1, 1 - (v - 0.5) / 0.26, 1);
        } else if (0.76 <= v && v < 0.85) {
            return Qt.rgba((v - 0.76) / 0.09, 1, 0, 1);
        } else if (0.85 <= v && v <= 1) {
            return Qt.rgba(1, 1 - (v - 0.85) / 0.15, 0, 1);
        } else {
            console.log("Invalid hueValue [0, 1]", hueValue);
            return "white";
        }
    }

    function hexToRgb(hex) {
        // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
        var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
        hex = hex.replace(shorthandRegex, function(m, r, g, b) {
            return r + r + g + g + b + b;
        });
        var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            "r": parseInt(result[1], 16),
            "g": parseInt(result[2], 16),
            "b": parseInt(result[3], 16)
        } : null;
    }

    function rgbToHex(r, g, b) {
        return ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
    }

    function drawChecker(ctx, width, height) {
        ctx.lineWidth = 0;
        //ctx.strokeStyle = 'blue'
        var numRows = Math.ceil(height / chekerSide);
        var numCols = Math.ceil(width / chekerSide);
        var lastWhite = false;
        var lastColWhite = false;
        for (var icol = 0; icol < numCols; icol++) {
            lastColWhite = lastWhite;
            for (var irow = 0; irow < numRows; irow++) {
                if (lastWhite)
                    ctx.fillStyle = 'gray';
                else
                    ctx.fillStyle = 'white';
                ctx.fillRect(icol * chekerSide, irow * chekerSide, chekerSide, chekerSide);
                lastWhite = !lastWhite;
            }
            lastWhite = !lastColWhite;
        }
    }

    function handleMouseVertSlider(mouse, ctrlCursor, ctrlHeight) {
        if (mouse.buttons & Qt.LeftButton) {
            ctrlCursor.y = Math.max(0, Math.min(ctrlHeight, mouse.y));
            setCurrentColor();
        }
    }

    function setCurrentColor() {
        var alphaFactor = 1 - (shpAlpha.y / shpAlpha.parent.height);
        if (optHsv.checked) {
            var hueFactor = 1 - (shpHue.y / shpHue.parent.height);
            hueColor = setHueColor(hueFactor);
            var saturation = (pickerCursor.x + colorHandleRadius) / pickerCursor.parent.width;
            var colorValue = 1 - ((pickerCursor.y + colorHandleRadius) / pickerCursor.parent.height);
            currentColor = Qt.hsva(hueFactor, saturation, colorValue, alphaFactor);
        } else {
            currentColor = Qt.rgba(sliderRed.value / 255, sliderGreen.value / 255, sliderBlue.value / 255, alphaFactor);
        }
        hexColor.text = rgbToHex(currentColor.r * 255, currentColor.g * 255, currentColor.b * 255);
        optHsv.focus = true;
    }

    function initColorRGB(ared, agreen, ablue, aalpha) {
        updatingControls = true;
        var acolor = Qt.rgba(ared / 255, agreen / 255, ablue / 255, aalpha / 255);
        var valHue = acolor.hsvHue;
        if (valHue < 0)
            valHue = 0;

        //console.log("toset", acolor.r * 255, acolor.g * 255, acolor.b * 255, acolor.a * 255)
        shpHue.y = ((1 - valHue) * shpHue.parent.height);
        shpAlpha.y = ((1 - acolor.a) * shpAlpha.parent.height);
        pickerCursor.x = (acolor.hsvSaturation * pickerCursor.parent.width) - colorHandleRadius;
        pickerCursor.y = ((1 - acolor.hsvValue) * pickerCursor.parent.height) - colorHandleRadius;
        sliderRed.value = ared;
        sliderGreen.value = agreen;
        sliderBlue.value = ablue;
        setCurrentColor();
        updatingControls = false;
    }

    width: 500
    height: 300
    Component.onCompleted: {
        initColorRGB(255, 0, 0, 255);
    }

    RowLayout {
        anchors.fill: parent
        //anchors.margins: marginsValue
        anchors.margins: 0
        spacing: marginsValue

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: marginsValue

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: optHsv.checked
                spacing: marginsValue

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Rectangle {
                        x: 0
                        y: 0
                        width: parent.width
                        height: parent.height
                        visible: true

                        gradient: Gradient {
                            orientation: Gradient.Horizontal

                            GradientStop {
                                position: 0
                                color: "#FFFFFF"
                            }

                            GradientStop {
                                position: 1
                                color: hueColor
                            }

                        }

                    }

                    Rectangle {
                        x: 0
                        y: 0
                        width: parent.width
                        height: parent.height
                        border.color: BppMetrics.accentColor
                        visible: true

                        gradient: Gradient {
                            GradientStop {
                                position: 1
                                color: "#FF000000"
                            }

                            GradientStop {
                                position: 0
                                color: "#00000000"
                            }

                        }

                    }

                    Rectangle {
                        id: pickerCursor

                        width: colorHandleRadius * 2
                        height: colorHandleRadius * 2
                        radius: colorHandleRadius
                        border.color: BppMetrics.windowBackground
                        border.width: 2
                        color: "transparent"

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 2
                            border.color: BppMetrics.accentColor
                            border.width: 2
                            radius: width / 2
                            color: "transparent"
                        }

                    }

                    MouseArea {
                        function handleMouse(mouse) {
                            if (mouse.buttons & Qt.LeftButton) {
                                pickerCursor.x = Math.max(-colorHandleRadius, Math.min(width, mouse.x) - colorHandleRadius);
                                pickerCursor.y = Math.max(-colorHandleRadius, Math.min(height, mouse.y) - colorHandleRadius);
                                setCurrentColor();
                            }
                        }

                        anchors.fill: parent
                        onPositionChanged: handleMouse(mouse)
                        onPressed: handleMouse(mouse)
                    }

                }

                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 30
                    border.color: BppMetrics.accentColor

                    Shape {
                        id: shpHue

                        anchors.left: parent.left
                        anchors.margins: 0
                        y: 90

                        ShapePath {
                            strokeWidth: 1
                            strokeColor: "black"

                            PathSvg {
                                path: "M0,0 L30,0"
                            }

                        }

                        ShapePath {
                            strokeWidth: 2
                            strokeColor: BppMetrics.accentColor
                            fillColor: "transparent"

                            PathSvg {
                                path: "M0,-5 L30,-5 L30,4 L0,4z"
                            }

                        }

                    }

                    MouseArea {
                        anchors.fill: parent
                        onPositionChanged: handleMouseVertSlider(mouse, shpHue, height)
                        onPressed: handleMouseVertSlider(mouse, shpHue, height)
                    }

                    gradient: Gradient {
                        GradientStop {
                            position: 1
                            color: "#FF0000"
                        }

                        GradientStop {
                            position: 0.85
                            color: "#FFFF00"
                        }

                        GradientStop {
                            position: 0.76
                            color: "#00FF00"
                        }

                        GradientStop {
                            position: 0.5
                            color: "#00FFFF"
                        }

                        GradientStop {
                            position: 0.33
                            color: "#0000FF"
                        }

                        GradientStop {
                            position: 0.16
                            color: "#FF00FF"
                        }

                        GradientStop {
                            position: 0
                            color: "#FF0000"
                        }

                    }

                }

            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: optRgb.checked

                Label {
                    text: qsTr("Red")
                    color: BppMetrics.textColor
                }

                Slider {
                    id: sliderRed

                    Layout.fillWidth: true
                    from: 0
                    to: 255
                    value: 0
                    onValueChanged: {
                        if (!updatingControls)
                            setCurrentColor();

                    }
                }

                Label {
                    text: qsTr("Green")
                    color: BppMetrics.textColor
                }

                Slider {
                    id: sliderGreen

                    Layout.fillWidth: true
                    from: 0
                    to: 255
                    value: 0
                    onValueChanged: {
                        if (!updatingControls)
                            setCurrentColor();

                    }
                }

                Label {
                    text: qsTr("Blue")
                    color: BppMetrics.textColor
                }

                Slider {
                    id: sliderBlue

                    Layout.fillWidth: true
                    from: 0
                    to: 255
                    value: 0
                    onValueChanged: {
                        if (!updatingControls)
                            setCurrentColor();

                    }
                }

                Item {
                    Layout.fillHeight: true
                }

            }

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumHeight: 25
                Layout.maximumHeight: 25
                border.color: BppMetrics.accentColor

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 1
                    spacing: 0

                    Repeater {
                        model: commonColors

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: modelData
                            border.color: "gray"
                            border.width: 0

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: {
                                    initColorRGB(parent.color.r * 255, parent.color.g * 255, parent.color.b * 255, 255);
                                }
                                onEntered: {
                                    border.width = 1;
                                    var compColor = Qt.rgba(1, 1, 1, 1);
                                    if (color.hsvValue > 0.5)
                                        compColor = Qt.rgba(0, 0, 0, 1);

                                    border.color = compColor;
                                }
                                onExited: {
                                    border.width = 0;
                                }
                            }

                        }

                    }

                }

            }

        }

        Canvas {
            Layout.fillHeight: true
            Layout.preferredWidth: 30
            onPaint: {
                var ctx = getContext('2d');
                drawChecker(ctx, width, height);
            }

            Rectangle {
                anchors.fill: parent
                border.color: BppMetrics.accentColor

                Shape {
                    id: shpAlpha

                    anchors.left: parent.left
                    anchors.margins: 0
                    y: 90

                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"

                        PathSvg {
                            path: "M0,0 L30,0"
                        }

                    }

                    ShapePath {
                        strokeWidth: 2
                        strokeColor: BppMetrics.accentColor
                        fillColor: "transparent"

                        PathSvg {
                            path: "M0,-5 L30,-5 L30,4 L0,4z"
                        }

                    }

                }

                MouseArea {
                    anchors.fill: parent
                    onPositionChanged: handleMouseVertSlider(mouse, shpAlpha, height)
                    onPressed: handleMouseVertSlider(mouse, shpAlpha, height)
                }

                gradient: Gradient {
                    //GradientStop { position: 0.0; color: "#FF000000" }
                    GradientStop {
                        position: 0
                        color: Qt.rgba(currentColor.r, currentColor.g, currentColor.b, 1)
                    }

                    GradientStop {
                        position: 1
                        color: "#00000000"
                    }

                }

            }

        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.minimumWidth: 70
            Layout.maximumWidth: 70

            //spacing: 0
            Canvas {
                Layout.fillWidth: true
                height: 35
                onPaint: {
                    var ctx = getContext('2d');
                    drawChecker(ctx, width, height);
                }

                Rectangle {
                    border.color: BppMetrics.accentColor
                    color: "transparent"
                    anchors.fill: parent

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 1
                        spacing: 0

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: Qt.rgba(currentColor.r, currentColor.g, currentColor.b, 1)
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: currentColor
                        }

                    }

                }

            }

            ColumnLayout {
                spacing: 0

                RadioButton {
                    id: optRgb

                    padding: 0
                    text: qsTr("RGB")
                    Layout.alignment: Qt.AlignLeft
                    onCheckedChanged: initColorRGB(currentColor.r * 255, currentColor.g * 255, currentColor.b * 255, currentColor.a * 255)

                    contentItem: Text {
                        text: optRgb.text
                        color: BppMetrics.textColor
                        font.bold: true
                        leftPadding: optRgb.indicator.width + optRgb.spacing
                        verticalAlignment: Text.AlignVCenter
                    }

                }

                RadioButton {
                    id: optHsv

                    padding: 0
                    text: qsTr("HSV")
                    Layout.alignment: Qt.AlignLeft
                    checked: true
                    focus: true
                    onCheckedChanged: initColorRGB(currentColor.r * 255, currentColor.g * 255, currentColor.b * 255, currentColor.a * 255)

                    contentItem: Text {
                        text: optHsv.text
                        color: BppMetrics.textColor
                        font.bold: true
                        leftPadding: optHsv.indicator.width + optHsv.spacing
                        verticalAlignment: Text.AlignVCenter
                    }

                }

            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                visible: optRgb.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("R:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.r * 255)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                visible: optRgb.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("G:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.g * 255)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                visible: optRgb.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("B:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.b * 255)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                visible: optHsv.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("H:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.hsvHue * 360)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                visible: optHsv.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("S:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.hsvSaturation * 100)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                visible: optHsv.checked
                Layout.fillWidth: true

                Label {
                    text: qsTr("V:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.hsvValue * 100)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            Item {
                Layout.fillHeight: true
            }

            //Label { text: qsTr("Alpha"); color: BppMetrics.textColor; font.bold: true; Layout.alignment: Qt.AlignLeft }
            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: qsTr("Alpha:")
                    color: BppMetrics.textColor
                }

                Label {
                    text: Math.floor(currentColor.a * 255)
                    Layout.fillWidth: true
                    color: BppMetrics.textColor
                }

            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 0

                Label {
                    text: qsTr("#")
                    color: BppMetrics.textColor
                }

                TextInput {
                    id: hexColor

                    Layout.fillWidth: true
                    text: "FF0099"
                    inputMask: "HHHHHH"
                    selectByMouse: true
                    color: BppMetrics.textColor
                    onTextChanged: {
                        if (!hexColor.focus)
                            return ;

                        if (!updatingControls && acceptableInput) {
                            //console.log('updating', rgbColor.r, currentColor.r * 255, rgbColor.g, currentColor.g * 255, rgbColor.b, currentColor.b * 255)

                            var rgbColor = hexToRgb(text);
                            if (rgbColor && rgbColor.r !== Math.floor(currentColor.r * 255) && rgbColor.g !== Math.floor(currentColor.g * 255) && rgbColor.b !== Math.floor(currentColor.b * 255))
                                initColorRGB(rgbColor.r, rgbColor.g, rgbColor.b, currentColor.a * 255);

                        }
                    }
                }

            }

        }

    }

}
