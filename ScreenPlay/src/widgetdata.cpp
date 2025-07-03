// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "widgetdata.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QObject>

namespace ScreenPlay {

QString WidgetData::loadErrorToString(LoadError error)
{
    switch (error) {
    case LoadError::None:
        return QObject::tr("No error");
    case LoadError::EmptyConfiguration:
        return QObject::tr("Empty widget configuration provided");
    case LoadError::WidgetFileDoesNotExist:
        return QObject::tr("Widget file specified in configuration does not exist");
    case LoadError::InvalidPosition:
        return QObject::tr("Invalid position specified for widget");
    }
    return QObject::tr("Unknown error");
}

QString WidgetData::toString() const
{
    return QStringLiteral(R"(WidgetData {
    title: %1
    absolutePath: %2
    previewImage: %3
    file: %4
    properties: %5
    type: %6
    position: (%7, %8)
})")
        .arg(title())
        .arg(absolutePath())
        .arg(previewImage())
        .arg(file())
        .arg(QString(QJsonDocument(properties()).toJson(QJsonDocument::Compact)))
        .arg(QString::number(static_cast<int>(type())))
        .arg(position().x())
        .arg(position().y());
}

bool WidgetData::hasContent()
{
    return InstalledContentData::hasContent();
}

QJsonObject WidgetData::serialize() const
{
    QJsonObject data = serializeBase();
    data.insert("x", position().x());
    data.insert("y", position().y());
    return data;
}

std::expected<WidgetData, WidgetData::LoadError> WidgetData::loadWidgetConfig(const QJsonObject& widgetObj)
{
    if (widgetObj.empty())
        return std::unexpected(LoadError::EmptyConfiguration);

    WidgetData widgetData;
    widgetData.loadBaseFromJson(widgetObj);
    
    // Load widget-specific properties
    int x = widgetObj.value("x").toInt(0);
    int y = widgetObj.value("y").toInt(0);
    widgetData.setPosition(QPoint(x, y));

    // Validate that the widget file exists
    if (!QFileInfo(widgetData.absolutePath() + "/" + widgetData.file()).exists()) {
        qWarning() << "Requested widget file does not exist";
        return std::unexpected(LoadError::WidgetFileDoesNotExist);
    }

    return widgetData;
}

}
