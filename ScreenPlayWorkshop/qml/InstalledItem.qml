import QtQuick
import QtQuick.Effects
import QtQuick.Controls

Item {
    id: root

    required property string preview
    required property string absoluteStoragePath
    required property string type
    // Must be var to make it work wit 64bit ints
    required property var publishedFileID
    required property int itemIndex
    required property string folderName
    property alias checkBox: checkBox
    property bool isSelected: false
    property bool hasMenuOpen: false

    signal itemClicked(var folderName, var type, var isActive)

    width: 320
    height: 180
    state: "invisible"
    opacity: 0
    onTypeChanged: {
        if (type === "widget")
            icnType.source = "icons/icon_widgets.svg";
        else if (type === "qmlScene")
            icnType.source = "icons/icon_code.svg";
    }
    Component.onCompleted: {
        root.state = "visible";
    }
    transform: [
        Rotation {
            id: rt

            origin.x: root.width * 0.5
            origin.y: root.height * 0.5
            angle: 0

            axis {
                x: -0.5
                y: 0
                z: 0
            }
        },
        Translate {
            id: tr
        },
        Scale {
            id: sc

            origin.x: root.width * 0.5
            origin.y: root.height * 0.5
        }
    ]

    Timer {
        id: timerAnim

        interval: 40 * root.itemIndex * Math.random()
        running: true
        repeat: false
        onTriggered: showAnim.start()
    }

    ParallelAnimation {
        id: showAnim

        running: false

        RotationAnimation {
            target: rt
            from: 90
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "angle"
        }

        PropertyAnimation {
            target: root
            from: 0
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            property: "opacity"
        }

        PropertyAnimation {
            target: tr
            from: 80
            to: 0
            duration: 500
            easing.type: Easing.OutQuint
            property: "y"
        }

        PropertyAnimation {
            target: sc
            from: 0.8
            to: 1
            duration: 500
            easing.type: Easing.OutQuint
            properties: "xScale,yScale"
        }
    }

    // Replaced RectangularGlow with MultiEffect
    MultiEffect {
        id: effect

        width: parent.width
        height: parent.height
        source: Item {
            anchors.fill: parent
        }
        shadowEnabled: true
        shadowBlur: 1.0
        shadowColor: "black"
        shadowOpacity: 0.4
        paddingRect: Qt.rect(-3, -3, 6, 6)

        anchors {
            top: parent.top
            topMargin: 3
        }
    }

    Item {
        id: screenPlayItemWrapper

        anchors.centerIn: parent
        height: 180
        width: 320

        Image {
            id: mask

            source: "qrc:/qt/qml/ScreenPlayWorkshop/assets/images/window.svg"
            sourceSize: Qt.size(root.width, root.height)
            visible: false
            smooth: true
            fillMode: Image.PreserveAspectFit
        }

        Item {
            id: itemWrapper

            anchors.fill: parent

            InstalledItemImage {
                id: screenPlayItemImage

                anchors.fill: parent
                sourceImage: Qt.resolvedUrl(root.absoluteStoragePath + "/" + root.preview)
            }

            Image {
                id: icnType

                width: 20
                height: 20
                opacity: 0
                sourceSize: Qt.size(20, 20)

                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
            }

            Rectangle {
                color: "#AAffffff"
                height: 30
                visible: false

                anchors {
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                }
            }
        }

        MultiEffect {
            id: maskEffect
            anchors.fill: itemWrapper
            source: itemWrapper
            maskEnabled: true
            maskSource: mask
            // Default values for other mask properties:
            maskSpreadAtMin: 0.0
            maskSpreadAtMax: 0.0
            maskThresholdMin: 0.0
            maskThresholdMax: 1.0
        }

        // Since MultiEffect can't contain MouseArea, we need to place it alongside
        MouseArea {
            anchors.fill: itemWrapper
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onEntered: {
                if (!root.hasMenuOpen)
                    root.state = "hover";
            }
            onExited: {
                if (!root.hasMenuOpen)
                    root.state = "visible";
            }
            onClicked: function (mouse) {
                checkBox.toggle();
                if (mouse.button === Qt.LeftButton)
                    itemClicked(root.folderName, root.type, checkBox.checkState === Qt.Checked);
            }
        }

        CheckBox {
            id: checkBox

            onCheckStateChanged: {
                if (checkState == Qt.Checked)
                    root.isSelected = true;
                else
                    root.isSelected = false;
            }

            anchors {
                top: parent.top
                right: parent.right
                margins: 10
            }
        }
    }

    states: [
        State {
            name: "invisible"

            PropertyChanges {
                screenPlayItemWrapper.y: -10
                screenPlayItemWrapper.opacity: 0
            }

            PropertyChanges {
                effect.opacity: 0
            }
        },
        State {
            name: "visible"

            PropertyChanges {
                effect.opacity: 0.4
            }

            PropertyChanges {
                screenPlayItemWrapper.y: 0
                screenPlayItemWrapper.opacity: 1
            }

            PropertyChanges {
                root.width: 320
                root.height: 180
            }

            PropertyChanges {
                icnType.opacity: 0
            }
        },
        State {
            name: "selected"

            PropertyChanges {
                screenPlayItemWrapper.y: 0
                screenPlayItemWrapper.opacity: 1
            }

            PropertyChanges {
                icnType.opacity: 0.5
            }
        }
    ]

    transitions: [
        Transition {
            from: "invisible"
            to: "visible"
        },
        Transition {
            from: "visible"
            to: "selected"
            reversible: true

            PropertyAnimation {
                target: icnType
                property: "opacity"
                duration: 80
            }
        }
    ]
}
