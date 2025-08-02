// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "installedcontentdata.h"
#include "ScreenPlayCore/util.h"
#include <QJsonDocument>
#include <QUrl>
#include <QVariant>

namespace ScreenPlay {

bool InstalledContentData::hasContent() const
{
    return !m_absolutePath.isEmpty() && !m_file.isEmpty();
}

QString InstalledContentData::toString() const
{
    return QStringLiteral(R"(InstalledContentData {
    title: %1
    absolutePath: %2
    previewImage: %3
    file: %4
    properties: %5
    type: %6
})")
        .arg(title())
        .arg(absolutePath())
        .arg(previewImage())
        .arg(file())
        .arg(QString(QJsonDocument(properties()).toJson(QJsonDocument::Compact)))
        .arg(ContentTypes::toString(type()));
}

QJsonObject InstalledContentData::serializeBase() const
{
    QJsonObject data;
    data.insert("title", title());
    data.insert("absolutePath", absolutePath());
    data.insert("previewImage", previewImage());
    data.insert("file", file());
    data.insert("properties", properties());
    data.insert("type", QVariant::fromValue(type()).toString());
    return data;
}

void InstalledContentData::loadBaseFromJson(const QJsonObject& obj)
{
    setTitle(obj.value("title").toString());
    setAbsolutePath(QUrl::fromUserInput(obj.value("absolutePath").toString()).toLocalFile());
    setPreviewImage(obj.value("previewImage").toString());
    setFile(obj.value("file").toString());
    setProperties(obj.value("properties").toObject());

    const QString typeString = obj.value("type").toString();
    setType(QStringToEnum<ContentTypes::InstalledType>(typeString,
        ContentTypes::InstalledType::VideoWallpaper));
}

}
