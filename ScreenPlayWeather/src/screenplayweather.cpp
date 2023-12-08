// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "screenplayweather.h"
#include "ScreenPlayUtil/util.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

ScreenPlayWeather::ScreenPlayWeather()
    : m_days(this)
{

    QObject::connect(this, &ScreenPlayWeather::cityChanged, this, &ScreenPlayWeather::updateLatitudeLongtitude);
    QObject::connect(this, &ScreenPlayWeather::latitudeLongtitudeChanged, this, &ScreenPlayWeather::update);
}

void ScreenPlayWeather::updateLatitudeLongtitude(const QString& city)
{
    auto request = defaultRequest();
    QUrl url = m_geocoding_open_metro_v1 + "search";
    QUrlQuery parameter;
    parameter.addQueryItem("name", city);
    url.setQuery(parameter);
    request.setUrl(url);
    auto* reply = m_networkAccessManager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        if (data.size() <= 0)
            return;

        const auto msgOpt = ScreenPlay::Util().parseQByteArrayToQJsonObject(data);
        if (!msgOpt.has_value())
            return;

        auto results = msgOpt->value("results").toArray();
        if (results.size() == 0)
            return;

        QJsonObject result = results.first().toObject();
        if (result.contains("latitude") && result.contains("longitude")) {
            setLatitude(result.value("latitude").toDouble());
            setLongtitude(result.value("longitude").toDouble());
            emit latitudeLongtitudeChanged();
        }

        if (result.contains("elevation"))
            setElevation(result.value("elevation").toInt());
        if (result.contains("population"))
            setPopulation(result.value("population").toInt());
    });
    QObject::connect(reply, &QNetworkReply::finished, this, []() { qInfo() << "updateLatitudeLongtitude finished!"; });
    QObject::connect(reply, &QNetworkReply::errorOccurred, this, []() { qInfo() << "errorOccurred!"; });
}

void ScreenPlayWeather::update()
{
    auto request = defaultRequest();
    const QString timezone = "Europe%2FLondon";
    const QString temperature_unit = QVariant::fromValue(m_temperatureUnit).toString().toLower();
    QString query = QString(
        "forecast?latitude=%1&"
        "longitude=%2&"
        "hourly=temperature_2m&"
        "daily=weathercode,"
        "temperature_2m_max,"
        "temperature_2m_min,"
        "sunrise,sunset,"
        "precipitation_sum,"
        "precipitation_hours&"
        "temperature_unit=celsius&"
        "timezone=%3")
                        .arg(m_latitude)
                        .arg(m_longtitude)
                        .arg(timezone);

    QUrl url = m_weather_open_metro_v1 + query;
    request.setUrl(url);
    qInfo() << url;
    auto* reply = m_networkAccessManager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        if (data.size() <= 0)
            return;

        ScreenPlay::Util util;
        const auto msgOpt = util.parseQByteArrayToQJsonObject(data);
        if (!msgOpt.has_value())
            return;

        if (msgOpt->contains("error")) {
            qWarning() << msgOpt->value("reason").toString();
            return;
        }

        const auto daily = msgOpt->value("daily").toObject();

        const auto sunrise = daily.value("sunrise").toArray();
        const auto sunset = daily.value("sunset").toArray();
        const auto time = daily.value("time").toArray();
        const auto weathercode = daily.value("weathercode").toArray();
        const auto temperature_2m_min = daily.value("temperature_2m_min").toArray();
        const auto temperature_2m_max = daily.value("temperature_2m_max").toArray();
        const auto precipitation_hours = daily.value("precipitation_hours").toArray();
        const auto precipitation_sum = daily.value("precipitation_sum").toArray();

        for (int i = 0; i < time.size(); ++i) {
            auto day = new Day();
            day->set_day(i);
            day->set_dateTime(QDateTime::fromString(time.at(i).toString(), m_dataTimeFormat));
            day->set_sunrise(QDateTime::fromString(sunrise.at(i).toString(), m_dataTimeFormat).toString("mm:ss"));
            day->set_sunset(QDateTime::fromString(sunset.at(i).toString(), m_dataTimeFormat).toString("mm:ss"));
            day->set_weatherCode(weathercode.at(i).toInt());
            day->set_temperature_2m_min(util.roundDecimalPlaces(temperature_2m_min.at(i).toDouble()));
            day->set_temperature_2m_max(util.roundDecimalPlaces(temperature_2m_max.at(i).toDouble()));
            day->set_precipitationHours(precipitation_hours.at(i).toInt());
            day->set_precipitationSum(util.roundDecimalPlaces(precipitation_sum.at(i).toDouble()));
            m_days.append(&m_days, std::move(day));
        }
        const auto hourly = msgOpt->value("hourly").toObject();
        emit ready();
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, []() { qInfo() << "errorOccurred!"; });
}

Day* ScreenPlayWeather::getDay(const int index) const
{
    //    if ((index >= m_days.count()) || index < 0) {
    //        qWarning() << "Invalid getDay index: " << index;
    //        return nullptr;
    //    }

    return nullptr; // m_days.at(index);
}

QNetworkRequest ScreenPlayWeather::defaultRequest() const
{
    QNetworkRequest request;
    const QString userAgent = "ScreenPlay";
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, userAgent);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    return request;
}

#include "moc_screenplayweather.cpp"
