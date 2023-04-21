import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlaySysInfo

Window {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlaySysInfo")

    property color accentColor: Material.secondaryTextColor
    property string fontFamily: "Arial"
    property int fontPointSize: 14

    AudioLevel {
        id: audioLevel
    }

    SysInfo {
        id: sysInfo
    }

    IpAddress {
        id: ipAddress
    }

    function stringListToString(list) {
        let out = "";
        for (var i = 0; i < list.length; i++) {
            out += "\n" + list[i];
        }
        return out;
    }

    Rectangle {
        anchors.fill: wrapper
        anchors.margins: -50
        color: Material.backgroundColor
        radius: 4

        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 4
        }
    }

    RowLayout {
        id: wrapper
        spacing: 40
        anchors.centerIn: parent
        ColumnLayout {
            id: wrapperLeft
            Layout.preferredWidth: 500

            ColumnLayout {
                spacing: 10
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "Private Addresses"
                    color: root.accentColor
                    font {
                        pointSize: 24
                        family: root.fontFamily
                    }
                }
                Text {
                    text: root.stringListToString(ipAddress.privateIpV4AddressList)
                    color: root.accentColor
                    font {
                        pointSize: 16
                        family: "Fira Code"
                    }
                }
                Text {
                    text: root.stringListToString(ipAddress.privateIpV6AddressList)
                    color: root.accentColor
                    font {
                        pointSize: 16
                        family: "Fira Code"
                    }
                }
                Item {
                    Layout.fillWidth: true
                }

                Rectangle {
                    color: root.accentColor
                    Layout.preferredHeight: 3
                    Layout.fillWidth: true
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "Public Addresses"
                    color: root.accentColor
                    font {
                        pointSize: 24
                        family: root.fontFamily
                    }
                }

                Text {
                    text: ipAddress.publicIpV4Address
                    color: root.accentColor
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                }
                Text {
                    text: ipAddress.publicIpV6Address
                    color: root.accentColor
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                }
            }
        }
        Rectangle {
            color: root.accentColor
            Layout.preferredHeight: 600
            width: 3
        }
        ColumnLayout {
            id: wrapperRight
            spacing: 20
            Layout.preferredWidth: 500
            Text {
                id: txtGPU
                text: "GPU"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 36
                horizontalAlignment: Text.AlignHCenter
            }

            RowLayout {
                Text {
                    text: sysInfo.gpu.vendor
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: "Fira Code"
                    }
                }
                Text {
                    text: sysInfo.gpu.name
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                }
            }

            Text {
                id: txtUptime
                text: "UPTIME"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 36
                horizontalAlignment: Text.AlignHCenter
            }

            RowLayout {
                id: valuesLayout
                spacing: 20
                Text {
                    text: "DAYS " + sysInfo.uptime.days
                    color: root.accentColor
                    font.family: root.fontFamily
                    font.pointSize: root.fontPointSize
                }
                Text {
                    text: "HOURS " + sysInfo.uptime.hours
                    color: root.accentColor
                    font.family: root.fontFamily
                    font.pointSize: root.fontPointSize
                }
                Text {
                    text: "MINUTES " + sysInfo.uptime.minutes
                    color: root.accentColor
                    font.family: root.fontFamily
                    font.pointSize: root.fontPointSize
                }

                Text {
                    id: txtSeconds
                    text: "SECONDS " + sysInfo.uptime.seconds
                    color: root.accentColor
                    font.family: root.fontFamily
                    font.pointSize: root.fontPointSize
                }
            }
            Text {
                id: txtCPU
                text: "CPU"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 36
                horizontalAlignment: Text.AlignHCenter
            }
            RowLayout {
                spacing: 10
                Layout.preferredWidth: 300
                Text {
                    id: txtCPUValue
                    text: Math.floor(sysInfo.cpu.usage)
                    color: root.accentColor
                    font.family: root.fontFamily
                    width: 70
                }
                ProgressBar {
                    from: 0
                    to: 100
                    value: Math.floor(sysInfo.cpu.usage)
                    Layout.fillWidth: true
                }
            }

            Text {
                id: txtStorage
                text: "STORAGE"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 36
                horizontalAlignment: Text.AlignHCenter
            }

            ListView {
                id: storageListView
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                model: sysInfo.storage
                delegate: Item {
                    width: 500
                    height: 40
                    RowLayout {
                        spacing: 10
                        width: 500

                        Text {
                            id: txtStorageName
                            text: name
                            color: root.accentColor
                            font.family: root.fontFamily
                            width: 70
                        }
                        ProgressBar {
                            from: 0
                            to: bytesTotal
                            value: bytesAvailable
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
