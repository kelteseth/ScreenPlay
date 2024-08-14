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
    /*!
        \brief Used in the "Installed" tab.
    */
public:
    enum class AppState {
        Inactive,
        Starting,
        Closing,
        Active,
    };
    Q_ENUM(AppState)
};

}
