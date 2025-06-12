import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Controls.Material.impl
import ScreenPlayWeather

Window {
    id: root
    width: 1600
    height: 768
    visible: true
    title: qsTr("ScreenPlayWeather")

    ScreenPlayWeather {
        id: weather
        city: "Friedrichshafen"
        onReady: {
            rp.model = weather.days;
            // Qt bug https://bugreports.qt.io/browse/QTBUG-105137
            test();
        }
    }
    function test() {
    }

    function mapWeatherCode(code) {
        const weather_time = ""; // or "-day", "-night"
        const weather_prefix = "wi" + weather_time + "-";
        // https://open-meteo.com/en/docs
        // WMO Weather interpretation codes (WW)
        // to https://erikflowers.github.io/weather-icons/
        switch (code) {
        case 0:
            return weather_prefix + "day-sunny";
        case 1:
        case 2:
        case 3:
            return weather_prefix + "cloud";
        case 45:
        case 48:
            return weather_prefix + "day-sunny";
        case 51:
        case 53:
        case 55:
            return weather_prefix + "rain-mix";
        case 61:
        case 63:
        case 65:
            return weather_prefix + "rain-mix";
        case 71:
        case 73:
        case 75:
            return weather_prefix + "snow";
        case 77:
            return weather_prefix + "snow";
        case 80:
        case 81:
        case 82:
            return weather_prefix + "snow";
        case 85:
        case 86:
            return weather_prefix + "snow";
        case 95:
            return weather_prefix + "thunderstorm";
        case 96:
        case 99:
            return weather_prefix + "storm-showers";
        }
    }
    Rectangle {
        anchors.fill: wrapper
        color: Material.backgroundColor
        Material.elevation: 5
        radius: 4
    }

    Rectangle {
        anchors.fill: wrapper
        color: Material.backgroundColor
        radius: 4

        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 4
        }
    }

    ColumnLayout {
        id: wrapper
        anchors.centerIn: parent
        width: implicitWidth + 100
        height: implicitHeight + 100
        TextField {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            onEditingFinished: weather.setCity(text)
            text: "Friedrichshafen"
        }
        Text {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            text: "longtitude: " + weather.longtitude + " - latitude: " + weather.latitude + " - elevation: " + weather.elevation + "m - population: " + weather.population
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Repeater {
                id: rp
                onModelChanged: print("MODEL CHANGED")
                onCountChanged: print(count)
                ColumnLayout {
                    id: cl
                    spacing: 20
                    Layout.preferredWidth: 120
                    TextItem {
                        text: "Day"
                        value: day
                    }
                    TextItem {
                        text: "Sunrise"
                        value: sunrise
                    }
                    TextItem {
                        text: "Sunset"
                        value: sunset
                    }

                    Image {
                        height: 64
                        width: height
                        sourceSize: Qt.size(height, height)
                        layer {
                            enabled: true
                            // effect: ColorOverlay {
                            //     color: Material.primaryColor
                            // }
                        }
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Image.AlignHCenter
                        source: "qrc:/qt/qml/ScreenPlayWeather/assets/icons/" + root.mapWeatherCode(weatherCode) + ".svg"
                    }
                    TextItem {
                        text: "Weather Code"
                        value: weatherCode
                    }
                    TextItem {
                        text: "Temperature min"
                        value: temperature_2m_min
                    }
                    TextItem {
                        text: "Temperature max"
                        value: temperature_2m_max
                    }

                    //                    TextItem {
                    //                        text: "Precipitation Sum"
                    //                        value: precipitationSum
                    //                    }

                    //                    TextItem {
                    //                        text: "Precipitation Hours"
                    //                        value: precipitationHours
                    //                    }
                }
            }
        }
    }

    component TextItem: Column {
        property alias value: value.text
        property alias text: description.text
        Layout.preferredWidth: 120
        Text {
            id: value
            width: 120
            font.pointSize: 16
            horizontalAlignment: Text.AlignHCenter
            color: Material.primaryTextColor
        }
        Text {
            id: description
            horizontalAlignment: Text.AlignHCenter
            color: Material.secondaryTextColor
            width: 120
        }
    }
}
