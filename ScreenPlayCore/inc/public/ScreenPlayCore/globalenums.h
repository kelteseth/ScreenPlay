// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QtCore/qmetatype.h>

namespace ScreenPlay {

class ScreenPlayEnums : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    /*!
        \brief
    */
    enum class AppState {
        Inactive,
        Starting,
        Closing,
        Active,
        Timeout,
    };
    Q_ENUM(AppState)
    /*!
        \brief
    */
    enum class Version {
        OpenSourceStandalone,
        OpenSourceSteam,
        OpenSourceProStandalone,
        OpenSourceProSteam,
        OpenSourceUltraStandalone,
        OpenSourceUltraSteam
    };
    Q_ENUM(Version)
};

}
