import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlaySysInfo

Window {
    id: root
    width: 1866  // Increased width to accommodate new column
    height: 868
    visible: true
    title: qsTr("ScreenPlaySysInfo")

    property color accentColor: Material.secondaryTextColor
    property string fontFamily: "Arial"
    property int fontPointSize: 14

    SysInfo {
        id: sysInfo
    }

    IpAddress {
        id: ipAddress
    }

    NetworkInterface {
        id: networkInterface
    }

    function stringListToString(list) {
        let out = "";
        for (var i = 0; i < list.length; i++) {
            out += "\n" + list[i];
        }
        return out;
    }

    function formatBytes(bytes) {
        if (bytes === 0)
            return '0 B';
        const k = 1024;
        const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    }

    function formatSpeed(bytesPerSecond) {
        if (bytesPerSecond === 0)
            return '0 B/s';
        const k = 1024;
        const sizes = ['B/s', 'KB/s', 'MB/s', 'GB/s'];
        const i = Math.floor(Math.log(bytesPerSecond) / Math.log(k));
        return parseFloat((bytesPerSecond / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
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

        // Left Column - IP Addresses
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
            Layout.preferredHeight: 700
            width: 3
        }

        // Middle Column - System Info
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

            ColumnLayout {
                spacing: 10
                Layout.fillWidth: true

                Text {
                    text: sysInfo.cpu.name || "CPU Name"
                    color: root.accentColor
                    font {
                        pointSize: 16
                        family: root.fontFamily
                    }
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    Text {
                        text: "Cores: " + sysInfo.cpu.numberOfCores
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: "Threads: " + sysInfo.cpu.threadCount
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: "Speed: " + sysInfo.cpu.maxClockSpeed + " MHz"
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }
                }

                RowLayout {
                    spacing: 10
                    Layout.fillWidth: true
                    Text {
                        id: txtCPUValue
                        text: Math.floor(sysInfo.cpu.usage) + "%"
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

        Rectangle {
            color: root.accentColor
            Layout.preferredHeight: 700
            width: 3
        }

        // New Right Column - Network Interface Stats
        ColumnLayout {
            id: wrapperNetwork
            spacing: 20
            Layout.preferredWidth: 500

            Text {
                text: "NETWORK"
                color: root.accentColor
                font.family: root.fontFamily
                font.pointSize: 36
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            ColumnLayout {
                spacing: 15
                Layout.fillWidth: true

                Text {
                    text: networkInterface.name
                    color: root.accentColor
                    font {
                        pointSize: 24
                        family: root.fontFamily
                    }
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                // Download Speed
                ColumnLayout {
                    spacing: 5
                    Layout.fillWidth: true

                    Text {
                        text: "Download Speed"
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: formatSpeed(networkInterface.downloadSpeed)
                        color: root.accentColor
                        font {
                            pointSize: 20
                            family: "Fira Code"
                        }
                    }
                }

                // Upload Speed
                ColumnLayout {
                    spacing: 5
                    Layout.fillWidth: true

                    Text {
                        text: "Upload Speed"
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: formatSpeed(networkInterface.uploadSpeed)
                        color: root.accentColor
                        font {
                            pointSize: 20
                            family: "Fira Code"
                        }
                    }
                }

                Rectangle {
                    color: root.accentColor
                    Layout.preferredHeight: 2
                    Layout.fillWidth: true
                }

                // Total Data Transfer
                Text {
                    text: "Total Data Transfer"
                    color: root.accentColor
                    font {
                        pointSize: 24
                        family: root.fontFamily
                    }
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                // Downloaded
                ColumnLayout {
                    spacing: 5
                    Layout.fillWidth: true

                    Text {
                        text: "Downloaded"
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: formatBytes(networkInterface.bytesReceived)
                        color: root.accentColor
                        font {
                            pointSize: 16
                            family: "Fira Code"
                        }
                    }
                }

                // Uploaded
                ColumnLayout {
                    spacing: 5
                    Layout.fillWidth: true

                    Text {
                        text: "Uploaded"
                        color: root.accentColor
                        font.family: root.fontFamily
                        font.pointSize: root.fontPointSize
                    }

                    Text {
                        text: formatBytes(networkInterface.bytesSent)
                        color: root.accentColor
                        font {
                            pointSize: 16
                            family: "Fira Code"
                        }
                    }
                }

                // Connection Status
                Text {
                    text: networkInterface.isActive ? "CONNECTED" : "DISCONNECTED"
                    color: networkInterface.isActive ? "#4CAF50" : "#F44336"
                    font {
                        pointSize: 16
                        family: root.fontFamily
                        bold: true
                    }
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
            }
        }
    }
}
