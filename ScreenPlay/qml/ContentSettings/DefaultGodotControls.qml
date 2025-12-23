import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlay
import ScreenPlayCore
import "qrc:/qt/qml/ScreenPlayCore/qml/InstantPopup.js" as InstantPopup

FocusScope {
    id: root
    state: "hidden"

    property bool timelineActive
    property int monitorIndex
    property int timelineIndex
    property string sectionIdentifier
    property var wallpaperData
    property bool hasContent: false

    onWallpaperDataChanged: {
        if (!wallpaperData)
            return
        root.hasContent = wallpaperData.hasContent()
        
        // Set FPS
        const currentFps = wallpaperData.godotFps
        for (let i = 0; i < cbFps.model.count; i++) {
            if (cbFps.model.get(i).value === currentFps) {
                cbFps.currentIndex = i
                break
            }
        }
        
        // Set 3D Scale Mode
        const currentScaleMode = wallpaperData.godot3DScaleMode
        for (let i = 0; i < cbScaleMode.model.count; i++) {
            if (cbScaleMode.model.get(i).value === currentScaleMode) {
                cbScaleMode.currentIndex = i
                break
            }
        }
        
        // Set 3D Scale
        sl3DScale.slider.value = wallpaperData.godot3DScale
    }

    Text {
        anchors.fill: parent
        anchors.margins: 20
        visible: !root.hasContent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: Material.secondaryTextColor
        text: qsTr("No timeline section selected")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        clip: true
        visible: root.hasContent

        Label {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            Layout.leftMargin: 10
            font.pointSize: 14
            wrapMode: Text.WrapAnywhere
            elide: Text.ElideRight
            color: root.timelineActive ? Material.primaryTextColor : Material.secondaryTextColor
            text: root.wallpaperData ? root.wallpaperData.title : ""
        }

        ColumnLayout {
            implicitHeight: 50
            spacing: 15
            Layout.fillWidth: true
            Layout.topMargin: 20
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            Text {
                id: txtFps
                height: 20
                font.pointSize: 14
                text: qsTr("FPS")
                verticalAlignment: Text.AlignVCenter
                font.family: App.settings.font
                color: Material.primaryTextColor
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
            }

            ComboBox {
                id: cbFps
                Layout.fillWidth: true
                Layout.leftMargin: 10
                textRole: "text"
                valueRole: "value"

                model: ListModel {
                    ListElement {
                        value: Godot.Fps.Fps1
                        text: qsTr("1 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps6
                        text: qsTr("6 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps12
                        text: qsTr("12 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps24
                        text: qsTr("24 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps30
                        text: qsTr("30 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps60
                        text: qsTr("60 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps120
                        text: qsTr("120 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Fps144
                        text: qsTr("144 FPS")
                    }
                    ListElement {
                        value: Godot.Fps.Unlimited
                        text: qsTr("Unlimited")
                    }
                    ListElement {
                        value: Godot.Fps.Vsync
                        text: qsTr("VSync")
                    }
                }
                onActivated: {
                    console.log(LoggingCategories.godotControls, "FPS changed - Timeline active:", root.timelineActive, "Monitor:", root.monitorIndex, "Timeline index:", root.timelineIndex, "Section:", root.sectionIdentifier, "New FPS:", cbFps.currentValue)
                    const category = ""
                    App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "godotFps", cbFps.currentValue, category).then(result => {
                        if (!result.success) {
                            InstantPopup.openErrorPopup(root, result.message)
                        }
                    })
                }
            }
        }

        ColumnLayout {
            implicitHeight: 50
            spacing: 15
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            Text {
                id: txtScaleMode
                height: 20
                font.pointSize: 14
                text: qsTr("3D Scaling Mode")
                verticalAlignment: Text.AlignVCenter
                font.family: App.settings.font
                color: Material.primaryTextColor
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
            }

            ComboBox {
                id: cbScaleMode
                Layout.fillWidth: true
                Layout.leftMargin: 10
                textRole: "text"
                valueRole: "value"

                model: ListModel {
                    ListElement {
                        value: Godot.ScaleMode3D.Bilinear
                        text: qsTr("Bilinear")
                    }
                    ListElement {
                        value: Godot.ScaleMode3D.FSR1_0
                        text: qsTr("FSR 1.0")
                    }
                    ListElement {
                        value: Godot.ScaleMode3D.FSR2_2
                        text: qsTr("FSR 2.2")
                    }
                }
                onActivated: {
                    console.log(LoggingCategories.godotControls, "3D Scale Mode changed - Timeline active:", root.timelineActive, "Monitor:", root.monitorIndex, "Timeline index:", root.timelineIndex, "Section:", root.sectionIdentifier, "New mode:", cbScaleMode.currentValue)
                    const category = ""
                    App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "godot3DScaleMode", cbScaleMode.currentValue, category).then(result => {
                        if (!result.success) {
                            InstantPopup.openErrorPopup(root, result.message)
                        }
                    })
                }
            }
        }

        LabelSlider {
            id: sl3DScale
            headline: qsTr("3D Render Scale")
            iconSource: "qrc:/qt/qml/ScreenPlay/assets/icons/icon_settings.svg"
            slider.from: 0.25
            slider.to: 2.0
            slider.stepSize: 0.05
            slider.value: 1.0
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            onValueEditingFinished: {
                const newScale = sl3DScale.slider.value.toFixed(2)
                console.log(LoggingCategories.godotControls, "3D Scale changed - Timeline active:", root.timelineActive, "Monitor:", root.monitorIndex, "Timeline index:", root.timelineIndex, "Section:", root.sectionIdentifier, "New scale:", newScale)
                const category = ""
                App.screenPlayManager.setValueAtMonitorTimelineIndex(root.monitorIndex, root.timelineIndex, root.sectionIdentifier, "godot3DScale", newScale, category).then(result => {
                    if (!result.success) {
                        InstantPopup.openErrorPopup(root, result.message)
                    }
                })
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    states: [
        State {
            name: "visible"

            PropertyChanges {
                target: root
                opacity: 1
                anchors.topMargin: 20
            }
        },
        State {
            name: "hidden"

            PropertyChanges {
                target: root
                opacity: 0
                anchors.topMargin: -50
            }
        }
    ]
    transitions: [
        Transition {
            from: "visible"
            to: "hidden"
            reversible: true

            PropertyAnimation {
                target: root
                duration: 300
                easing.type: Easing.InOutQuart
                properties: "anchors.topMargin, opacity"
            }
        }
    ]
}
