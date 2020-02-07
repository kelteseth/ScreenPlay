import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import Qt.labs.platform 1.0
import QtQuick.Layouts 1.12

import ScreenPlay 1.0
import ScreenPlay.Create 1.0

import "../../../Common"

Item {
    id: root

    SwipeView {
        id: swipeView
        anchors.fill: parent
        interactive: false
        clip: true

        Item {
            Item {
                width: parent.width * .66
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                    margins: 20
                }
                Image {
                    source: "qrc:/assets/images/undraw_static_website_0107.svg"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    width: parent.width
                    height: 300
                    fillMode: Image.PreserveAspectFit
                }
            }

            Item {
                width: parent.width * .33
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }

                Text {
                    id: txtDescription
                    text: qsTr("This wizard lets you create a empty html based wallpaper. You can put anything you can imagine into this html file. For example this can be a three.js scene or a utility application written in javascript.")
                    color: "gray"
                    width: parent.width - 40
                    font.pointSize: 13

                    anchors {
                        centerIn: parent
                    }

                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            Button {
                text: qsTr("Next")
                highlighted: true
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }

                onClicked: swipeView.incrementCurrentIndex()
            }
        }
        Item {

            Item {
                width: parent.width * .5
                anchors {
                    top: parent.top
                    left: parent.left
                    bottom: parent.bottom
                    margins: 20
                }
                Image {
                    source: "qrc:/assets/images/undraw_static_website_0107.svg"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    width: parent.width
                    height: 400
                    fillMode: Image.PreserveAspectFit
                }
            }
            Item {
                width: parent.width * .5
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    margins: 20
                }

                ColumnLayout {
                    id: rightWrapper
                    spacing: 8
                    anchors {
                        top: parent.top
                        right: parent.right
                        left: parent.left
                    }

                    Headline {
                        text.text: qsTr("Create a html Wallpaper")
                    }

                    Text {
                        text: qsTr("General")
                        font.pointSize: 14
                        color: "#757575"
                    }
                    TextField {
                        id: tfTitle
                        Layout.fillWidth: true
                        placeholderText: qsTr("Wallpaper name")
                        onTextChanged: {
                            if (text.length >= 3) {
                                btnSave.enabled = true
                            } else {
                                btnSave.enabled = false
                            }
                        }
                    }
                    TextField {
                        id: tfCreatedBy
                        Layout.fillWidth: true
                        placeholderText: qsTr("Copyright owner")
                    }

                    Text {
                        text: qsTr("License")
                        font.pointSize: 14
                        color: "#757575"
                    }
                    ComboBox {
                        id: cbLicense
                        Layout.fillWidth: true
                        model: ListModel {
                            id: modelLicense
                            ListElement {
                                text: "All rights "
                            }
                            ListElement {
                                text: "Open Source - GPLv3"
                            }
                            ListElement {
                                text: "Open Source - MIT/Apache2"
                            }
                        }
                    }
                    Text {
                        text: qsTr("Tags")
                        font.pointSize: 14
                        color: "#757575"
                    }

                    TagSelector {
                        id: tagSelector
                        Layout.fillWidth: true
                    }
                    Text {
                        text: qsTr("Preview Image")
                        font.pointSize: 14
                        color: "#757575"
                    }
                    ImageSelector {
                        id: previewSelector
                        placeHolderText: qsTr("You can set your own preview image here!")
                        Layout.fillWidth: true
                    }
                }
            }
            Row {
                height: 80

                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    rightMargin: 20
                }

                spacing: 10

                Connections {
                    target: ScreenPlay.create
                    onHtmlWallpaperCreatedSuccessful: {
                        ScreenPlay.util.openFolderInExplorer(path)
                    }
                }

                Button {
                    id: btnExit
                    text: qsTr("Abort")
                    Material.background: Material.Red
                    Material.foreground: "white"
                    onClicked: {
                        ScreenPlay.util.setNavigationActive(true)
                        ScreenPlay.util.setNavigation("Create")
                    }
                }

                Button {
                    id: btnSave
                    text: qsTr("Save")
                    enabled: false
                    Material.background: Material.Orange
                    Material.foreground: "white"

                    onClicked: {
                        btnSave.enabled = false
                        savePopup.open()
                        var tags = tagSelector.getTags()
                        ScreenPlay.create.createHTMLWallpaper(
                                    ScreenPlay.globalVariables.localStoragePath,
                                    tfTitle.text,
                                    previewSelector.imageSource,
                                    cbLicense.currentText,
                                    tags)
                    }
                }
            }
        }
    }

    Popup {
        id: savePopup
        modal: true
        focus: true
        width: 250
        anchors.centerIn: parent
        height: 200
        onOpened: timerSave.start()

        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }
        Text {
            text: qsTr("Create Html Wallpaper...")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
        }

        Timer {
            id: timerSave
            interval: 1000 + Math.random() * 1000
            onTriggered: {
                savePopup.close()
                ScreenPlay.util.setNavigationActive(true)
                ScreenPlay.util.setNavigation("Installed")
            }
        }
    }
    PageIndicator {
        id: indicator

        count: swipeView.count
        currentIndex: swipeView.currentIndex

        anchors {
            bottom: swipeView.bottom
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

