import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import ScreenPlayWorkshop as SP

Popup {
    id: root

    dim: true
    width: 800
    height: 400
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: Overlay.overlay

    ColumnLayout {
        anchors {
            fill: parent
            margins: 20
        }

        Text {
            text: qsTr("You Need to Agree To The Steam Subscriber Agreement First")
            Layout.fillWidth: true
        }

        Text {
            id: name

            text: qsTr("REQUIRES INTERNET CONNECTION AND FREE STEAM ACCOUNT TO ACTIVATE. Notice: Product offered subject to your acceptance of the Steam Subscriber Agreement (SSA). You must activate this product via the Internet by registering for a Steam account and accepting the SSA. Please see https://store.steampowered.com/subscriber_agreement/ to view the SSA prior to purchase. If you do not agree with the provisions of the SSA, you should return this game unopened to your retailer in accordance with their return policy.")
            color: Material.primaryTextColor
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: btnAbort

                text: qsTr("View The Steam Subscriber Agreement")
                onClicked: Qt.openUrlExternally("https://store.steampowered.com/subscriber_agreement/")
            }

            Button {
                id: btnAgree

                text: qsTr("Accept Steam Workshop Agreement")
                Material.accent: Material.color(Material.Orange)
                highlighted: true
                onClicked: {
                    Qt.openUrlExternally("https://steamcommunity.com/sharedfiles/workshoplegalagreement");
                    root.close();
                }
            }
        }
    }

    background: Rectangle {
        color: Material.theme === Material.Light ? "white" : Material.background
    }
}
