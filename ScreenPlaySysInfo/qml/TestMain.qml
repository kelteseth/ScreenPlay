import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlaySysInfo

Window {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlaySysInfo")
    color: "#19181E"

    property color accentColor: "#FF9800"
    property string fontFamily: "Arial"
    property int fontPointSize: 16

    SysInfo {
        id: sysInfo
    }

    IpAddress {
        id: ipAddress
    }

    function stringListToString(list) {
        let out = ""
        for (var i = 0; i < list.length; i++) {
            out += "\n" + list[i]
        }
        return out
    }

    RowLayout {
        spacing: 40
        anchors.fill: parent
        anchors.margins: 20

        ColumnLayout {
            id: wrapperLeft
            Text {
                text: "IpAddress"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 60
                horizontalAlignment: Text.AlignHCenter
            }

            ColumnLayout {
                Text {
                    text: root.stringListToString(
                              ipAddress.privateIpV4AddressList)
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: "Fira Code"
                    }
                }
                Text {
                    text: root.stringListToString(
                              ipAddress.privateIpV6AddressList)
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: "Fira Code"
                    }
                }
                Rectangle {
                    color: root.accentColor
                    Layout.preferredHeight: 5
                    Layout.fillWidth: true
                }

                Text {
                    text: ipAddress.publicIpV4Address
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                }
                Text {
                    text: ipAddress.publicIpV6Address
                    color: root.accentColor
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                }
            }
        }
        Rectangle {
            color: root.accentColor
            Layout.fillHeight: true
            width: 10
        }
        ColumnLayout {
            id: wrapperRight

            Text {
                id: txtGPU
                text: "GPU"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 60
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
                font.pointSize: 60
                horizontalAlignment: Text.AlignHCenter
            }

            RowLayout {
                id: valuesLayout
                spacing: 20
                Text {
                    id: txtYears
                    text: "YEARS " + sysInfo.uptime.years
                    color: root.accentColor
                    font.family: root.fontFamily
                    font.pointSize: root.fontPointSize
                }
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
                font.pointSize: 60
                horizontalAlignment: Text.AlignHCenter
            }
            Row {
                id: row
                spacing: 10
                Layout.fillWidth: true
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
                font.pointSize: 60
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 20
            }

            ListView {
                id: storageListView
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                model: sysInfo.storage
                delegate: Item {
                    width: storageListView.width
                    height: 40
                    Row {
                        spacing: 10
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
                            width: storageListView.width - txtStorageName.width - row.spacing
                        }
                    }
                }
            }
        }
    }
}
