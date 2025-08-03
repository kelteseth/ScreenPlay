// SPDX-License-Identifier: BSD-3-Clause
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWeather
import ScreenPlayCore as Core

Item {
    id: root
    implicitWidth: 900
    implicitHeight: 650

    ScreenPlayWeather {
        id: weather
        city: "Friedrichshafen"
        onReady: {
            rp.model = weather.days;
        }
    }

    function mapWeatherCode(code) {
        const weather_time = "";
        // or "-day", "-night"
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

    ColumnLayout {
        id: wrapper
        anchors.fill: parent
        TextField {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            onEditingFinished: weather.setCity(text)
            text: "Friedrichshafen"
        }

        Label {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            text: "longtitude: " + weather.longtitude + " - latitude: " + weather.latitude + " - elevation: " + weather.elevation + "m - population: " + weather.population
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Repeater {
                id: rp
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

                    Core.ColorImage {
                        height: 64
                        width: height
                        sourceSize: Qt.size(height, height)
                        color: Material.primaryColor
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
                }
            }
        }
    }

    component TextItem: Column {
        property alias value: value.text
        property alias text: description.text
        Layout.preferredWidth: 120
        Label {
            id: value
            width: 120
            font.pointSize: 16
            color: Material.primaryTextColor
            horizontalAlignment: Text.AlignHCenter
        }
        Label {
            id: description
            horizontalAlignment: Text.AlignHCenter
            color: Material.secondaryTextColor
            width: 120
        }
    }
}
