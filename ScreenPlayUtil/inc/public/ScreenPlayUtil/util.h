// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlayUtil/contenttypes.h"
#include <QtGlobal>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QMetaType>
#include <QString>
#include <QVersionNumber>
#include <optional>

namespace ScreenPlayUtil {

QJsonArray fillArray(const QVector<QString>& items);
ScreenPlay::SearchType::SearchType getSearchTypeFromInstalledType(const ScreenPlay::InstalledType::InstalledType type);
std::optional<ScreenPlay::InstalledType::InstalledType> getInstalledTypeFromString(const QString& type);
std::optional<ScreenPlay::VideoCodec::VideoCodec> getVideoCodecFromString(const QString& type);
std::optional<QJsonObject> parseQByteArrayToQJsonObject(const QByteArray& byteArray);
std::optional<QJsonObject> openJsonFileToObject(const QString& path);
std::optional<QString> openJsonFileToString(const QString& path);
std::optional<QVersionNumber> getVersionNumberFromString(const QString& str);
bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
bool writeSettings(const QJsonObject& obj, const QString& absolutePath);
bool writeFile(const QString& text, const QString& absolutePath);
bool writeFileFromQrc(const QString& qrcPath, const QString& absolutePath);
bool copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination);
QString toString(const QStringList& list);
QString toLocal(const QString& url);
QString generateRandomString(quint32 length = 32);
QString executableAppEnding();
QString executableBinEnding();
QStringList getAvailableWallpaper();
QStringList getAvailableWidgets();
QStringList getAvailableTypes();
QStringList getAvailableFillModes();
bool isWallpaper(const ScreenPlay::InstalledType::InstalledType type);
bool isWidget(const ScreenPlay::InstalledType::InstalledType type);
std::optional<QVector<int>> parseStringToIntegerList(const QString string);
float roundDecimalPlaces(const float number);
}
