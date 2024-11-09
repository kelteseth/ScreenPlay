// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once
#include "ScreenPlayCore/PropertyHelpers.h"
#include <QDateTime>
#include <QObject>
#include <QQmlEngine>

class Day : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    W_PROPERTY(int, day)
    W_PROPERTY(QDateTime, dateTime)
    W_PROPERTY(QString, sunrise)
    W_PROPERTY(QString, sunset)
    W_PROPERTY_DEFAULT(int, weatherCode, 0)
    W_PROPERTY_DEFAULT(float, temperature_2m_min, 0.0f)
    W_PROPERTY_DEFAULT(float, temperature_2m_max, 0.0f)
    W_PROPERTY_DEFAULT(int, precipitationHours, 0)
    W_PROPERTY_DEFAULT(float, precipitationSum, 0.0f)
};
