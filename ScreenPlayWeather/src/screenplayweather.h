// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include "ScreenPlayUtil/ListPropertyHelper.h"
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include <memory>

#include "day.h"

class ScreenPlayWeather : public QObject {
    Q_OBJECT

    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longtitude READ longtitude WRITE setLongtitude NOTIFY longtitudeChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(int elevation READ elevation WRITE setElevation NOTIFY elevationChanged)
    Q_PROPERTY(int population READ population WRITE setPopulation NOTIFY populationChanged)
    QML_ELEMENT

    LIST_PROPERTY(Day, days)

public:
    explicit ScreenPlayWeather();

    double latitude() const { return m_latitude; }
    double longtitude() const { return m_longtitude; }
    const QString& city() const { return m_city; }
    int elevation() const { return m_elevation; }
    int population() const { return m_population; }

    enum class TemperatureUnit {
        Celsius,
        Fahrenheit
    };

public slots:
    Day* getDay(const int index) const;

    void setLatitude(double latitude)
    {
        if (qFuzzyCompare(m_latitude, latitude))
            return;
        m_latitude = latitude;
        emit latitudeChanged(m_latitude);
    }

    void setLongtitude(double longtitude)
    {
        if (qFuzzyCompare(m_longtitude, longtitude))
            return;
        m_longtitude = longtitude;
        emit longtitudeChanged(m_longtitude);
    }

    void setCity(const QString& city)
    {
        if (m_city == city)
            return;
        m_city = city;
        emit cityChanged(m_city);
    }

    void setElevation(int elevation)
    {
        if (m_elevation == elevation)
            return;
        m_elevation = elevation;
        emit elevationChanged(m_elevation);
    }

    void setPopulation(int population)
    {
        if (m_population == population)
            return;
        m_population = population;
        emit populationChanged(m_population);
    }

private slots:
    void updateLatitudeLongtitude(const QString& city);
    void update();

signals:
    void latitudeChanged(double latitude);
    void longtitudeChanged(double longtitude);
    void cityChanged(const QString& city);
    void latitudeLongtitudeChanged();
    void elevationChanged(int elevation);
    void populationChanged(int population);
    void ready();

private:
    QNetworkRequest defaultRequest() const;

private:
    QNetworkAccessManager m_networkAccessManager;
    double m_latitude = 0.0l;
    double m_longtitude = 0.0l;
    QString m_city;
    const QString m_geocoding_open_metro_v1 = "https://geocoding-api.open-meteo.com/v1/";
    const QString m_weather_open_metro_v1 = "https://api.open-meteo.com/v1/";
    const QString m_dataTimeFormat = "yyyy-MM-ddTmm:ss";
    int m_elevation;
    int m_population;
    int FORCAST_DAYS = 6;
    TemperatureUnit m_temperatureUnit = TemperatureUnit::Celsius;
};
