import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.3
import net.aimber.create 1.0

Item {
    id: createNew
    anchors.fill: parent
    state: "out"

    property string filePath

    //Blocks some MouseArea from create page
    MouseArea {
        anchors.fill: parent
    }

    Connections {
        target: screenPlayCreate
        onCreateWallpaperStateChanged: {
            print(state)
            if (state === Create.State.ConvertingPreviewGifError ||
                    state === Create.State.ConvertingPreviewVideoError) {
                createNew.state = "error"
            }
        }
    }

    Timer {
        interval: 1000
        triggeredOnStart: false
        running: true
        repeat: false
        onTriggered: {
            screenPlayCreate.createWallpaperStart(filePath)
        }
    }

    Component.onCompleted: {
        state = "in"
    }

    RectangularGlow {
        id: effect
        anchors {
            top: wrapper.top
            left: wrapper.left
            right: wrapper.right
            topMargin: 3
        }

        height: wrapper.height
        width: wrapper.width
        cached: true
        glowRadius: 3
        spread: 0.2
        color: "black"
        opacity: 0.4
        cornerRadius: 15
    }

    Rectangle {
        id: wrapper

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
        }

        width: 910
        radius: 4
        height: 560

        Item {
            id: wrapperSteps
            anchors.fill: parent

            Text {
                id: txtHeadline
                text: qsTr("Convert a video to a wallpaper")
                height: 40
                font.family: "Roboto"
                font.weight: Font.Light
                color: "#757575"
                renderType: Text.NativeRendering
                font.pixelSize: 23
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 40
                    bottomMargin: 0
                }
            }

            SwipeView {
                id: view
                clip: true
                currentIndex: 0
                interactive: false
                onCurrentIndexChanged: {

                }

                anchors {
                    top: txtHeadline.bottom
                    right: parent.right
                    bottom: indicator.top
                    left: parent.left
                    margins: 40
                    bottomMargin: 20
                    topMargin: 0
                }

                Page_0 {
                    id: page_0
                    onCanNextChanged: {
                        if (canNext) {
                            btnNext.state = "enabledPage0"
                        } else {
                            if (gifCreated) {
                                btnNext.state = "diabledPage0NoTitle"
                            } else {
                                btnNext.state = "diabledPage0"
                            }
                        }
                    }
                    onGifCreatedChanged: {
                        if (gifCreated) {
                            btnNext.state = "diabledPage0NoTitle"
                        }
                    }
                }
                Page_1 {
                    id: secondPage
                }
                Page_2 {
                    id: thirdPage
                }
            }

            PageIndicator {
                id: indicator
                count: view.count
                currentIndex: view.currentIndex
                anchors {
                    bottom: parent.bottom
                    bottomMargin: 20
                    left: parent.left
                    leftMargin: 40
                }

                delegate: Item {
                    width: txtStep.paintedWidth + 20
                    height: 30
                    property bool filled
                    Text {
                        id: txtStep
                        text: {
                            switch (index) {
                            case 0:
                                return "1. Configure"
                            case 1:
                                return "2. Convert"
                            case 2:
                                return "3. Finish"
                            default:
                                return "Undefiend"
                            }
                        }
                        color: view.currentIndex == index ? "orange" : "gray"
                        renderType: Text.NativeRendering
                        font.family: "Roboto"
                        font.pixelSize: 14
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {

                                if (!page_0.canNext || !page_0.gifCreated)
                                    return

                                view.setCurrentIndex(index)
                            }

                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                }
            }

            Row {
                width: childrenRect.width
                height: 50
                spacing: 20
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    margins: 20
                    rightMargin: 40
                }
                Button {
                    text: qsTr("Back")
                    Material.background: Material.Gray
                    Material.foreground: "white"

                    icon.source: "qrc:/assets/icons/icon_arrow_left.svg"
                    icon.color: "white"
                    icon.width: 16
                    icon.height: 16
                    onClicked: {
                        if (view.currentIndex > 0)
                            view.setCurrentIndex(view.currentIndex - 1)
                    }
                }
                NextButton {
                    id: btnNext
                    state: "diabledPage0"
                    onClicked: {

                        if (!page_0.canNext || !page_0.gifCreated)
                            return

                        if (view.currentIndex < view.count - 1)
                            view.setCurrentIndex(view.currentIndex + 1)
                    }
                }
            }

            MouseArea {
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 5
                }
                width: 32
                height: width
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    createNew.state = "out"
                    timerBack.start()
                }

                Image {
                    id: imgClose
                    source: "qrc:/assets/icons/font-awsome/close.svg"
                    width: 16
                    height: 16
                    anchors.centerIn: parent
                    sourceSize: Qt.size(width, width)
                }
                ColorOverlay {
                    id: iconColorOverlay
                    anchors.fill: imgClose
                    source: imgClose
                    color: "gray"
                }
                Timer {
                    id: timerBack
                    interval: 800
                    onTriggered: utility.setNavigation("Create")
                }
            }
        }
    }

    states: [
        State {
            name: "out"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 800
                opacity: 0
            }
            PropertyChanges {
                target: effect
                opacity: 0
            }
        },
        State {
            name: "in"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 40
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: wrapper
                anchors.topMargin: 40
                opacity: 1
            }
            PropertyChanges {
                target: effect
                opacity: .4
            }
            PropertyChanges {
                target: wrapperSteps
                opacity: 0
            }
        }
    ]
    transitions: [
        Transition {
            from: "out"
            to: "in"
            SequentialAnimation {

                PauseAnimation {
                    duration: 400
                }
                ParallelAnimation {

                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "anchors.topMargin"
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: wrapper
                        duration: 600
                        property: "opacity"
                        easing.type: Easing.InOutQuad
                    }
                    SequentialAnimation {

                        PauseAnimation {
                            duration: 1000
                        }
                        PropertyAnimation {
                            target: effect
                            duration: 300
                            property: "opacity"
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "out"

            ParallelAnimation {

                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "anchors.topMargin"
                    easing.type: Easing.InOutQuad
                }
                PropertyAnimation {
                    target: wrapper
                    duration: 600
                    property: "opacity"
                    easing.type: Easing.InOutQuad
                }
                SequentialAnimation {

                    PauseAnimation {
                        duration: 500
                    }
                    PropertyAnimation {
                        target: effect
                        duration: 300
                        property: "opacity"
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        },
        Transition {
            from: "in"
            to: "error"
            PropertyAnimation {
                target: wrapperSteps
                duration: 600
                property: "opacity"
                easing.type: Easing.InOutQuad
            }
        }
    ]
}
