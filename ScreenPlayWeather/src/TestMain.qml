import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import ScreenPlayWeather 1.0

Window {
    id: root
    width: 1366
    height: 768
    visible: true
    title: qsTr("ScreenPlayWeather")

    ScreenPlayWeather {
        id: weather
        city: "Friedrichshafen"
        onReady: {

            rp.model = weather.days
            print("onReady" ,weather.days.count)
//            for (var i = 0; i < rp.count; i++) {
//                rp.itemAt(i).day = weather.getDay(i)
//            }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        TextField {
            Layout.alignment: Qt.AlignHCenter
            onEditingFinished: weather.setCity(text)
            text: "Friedrichshafen"
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
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
                        horizontalAlignment: Text.AlignHCenter
                        text: day + "\nday"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: dateTime + "\ndateTime"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: sunrise + "\nsunrise"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: sunset + "\nsunset"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: weatherCode + "\nweatherCode"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text:temperature_2m_min + "\ntemperature_2m_min"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: temperature_2m_max + "\ntemperature_2m_max"
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        text: precipitationHours + "\nprecipitationHours"
                    }
                    Text {
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
