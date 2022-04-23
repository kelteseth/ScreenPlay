import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWeather 1.0

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

            rp.model = weather.days
            print("onReady", weather.days.count)
            //            for (var i = 0; i < rp.count; i++) {
            //                rp.itemAt(i).day = weather.getDay(i)
            //            }
        }
    }
    function mapWeatherCode(code) {
        const weather_time = "" // or "-day", "-night"
        const weather_prefix = "wi" + weather_time + "-"
        // https://open-meteo.com/en/docs
        // WMO Weather interpretation codes (WW)
        // to https://erikflowers.github.io/weather-icons/
        switch (code) {
        case 0:
            return time + "-sunny"
        case 1:
        case 2:
        case 3:
            return weather_prefix + "cloud"
        case 45:
        case 48:
            return weather_prefix + "sunny"
        case 51:
        case 53:
        case 55:
            return weather_prefix + "rain-mix"
        case 61:
        case 63:
        case 65:
            return weather_prefix + "rain-mix"
        case 71:
        case 73:
        case 75:
            return weather_prefix + "snow"
        case 77:
            return weather_prefix + "snow"
        case 80:
        case 81:
        case 82:
            return weather_prefix + "snow"
        case 85:
        case 86:
            return weather_prefix + "snow"
        case 95:
            return weather_prefix + "thunderstorm"
        case 96:
        case 99:
            return weather_prefix + "storm-showers"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        TextField {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            onEditingFinished: weather.setCity(text)
            text: "Friedrichshafen"
        }
        Text {
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            text: "longtitude: " + weather.longtitude + " - latitude: "
                  + weather.latitude + " - elevation: " + weather.elevation
                  + "m - population: " + weather.population
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Repeater {
                id: rp
                onCountChanged: print(count)
                ColumnLayout {
                    id: cl
                    spacing: 20
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: day + "\nday"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: dateTime + "\ndateTime"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: sunrise + "\nsunrise"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: sunset + "\nsunset"
                    }

                    Image {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Image.AlignHCenter
                        source: "qrc:/ScreenPlayWeather/assets/icons/" + root.mapWeatherCode(
                                    weatherCode) + ".svg"
                    }

                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: weatherCode + "\nweatherCode"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: temperature_2m_min + "\ntemperature_2m_min"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: temperature_2m_max + "\ntemperature_2m_max"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: precipitationHours + "\nprecipitationHours"
                    }
                    Text {
                        Layout.alignment: Qt.AlignCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: precipitationSum + "\nprecipitationSum"
                    }
                }
            }
        }
        Button {
            text: "getDay"
            onClicked: {
                var day = weather.getDay(1)
                print("weatherCode:", day.weatherCode, day.precipitationSum)
            }
        }
    }
}
