// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlay/installedcontentdata.h"
#include <QJsonObject>
#include <QPoint>
#include <expected>

namespace ScreenPlay {

/*!
    \brief Data class for widget content.

    This class extends InstalledContentData with widget-specific properties
    like position. It provides serialization, deserialization, and validation
    for widget configurations.
*/
class WidgetData : public InstalledContentData {
    Q_GADGET
    QML_ANONYMOUS
    Q_PROPERTY(QPoint position READ position WRITE setPosition)

public:
    enum class LoadError {
        None,
        EmptyConfiguration,
        WidgetFileDoesNotExist,
        InvalidPosition
    };
    Q_ENUM(LoadError)

    static QString loadErrorToString(LoadError error);

    // Widget-specific properties
    QPoint position() const { return m_position; }
    void setPosition(const QPoint& position) { m_position = position; }

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE bool hasContent();

    QJsonObject serialize() const;
    static std::expected<WidgetData, LoadError> loadWidgetConfig(const QJsonObject& widgetObj);

private:
    QPoint m_position;
};

}
