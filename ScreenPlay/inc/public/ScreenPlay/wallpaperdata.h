// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlayCore/contenttypes.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QUuid>

namespace ScreenPlay {

class WallpaperData {
    Q_GADGET
    QML_ANONYMOUS
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage)
    Q_PROPERTY(float volume READ volume WRITE setVolume)
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(QJsonObject properties READ properties WRITE setProperties)
    Q_PROPERTY(ScreenPlay::ContentTypes::InstalledType type READ type WRITE setType)
    Q_PROPERTY(ScreenPlay::Video::FillMode fillMode READ fillMode WRITE setFillMode)
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors)
public:
    bool isLooping() const { return m_isLooping; }
    QString absolutePath() const { return m_absolutePath; }
    QString previewImage() const { return m_previewImage; }
    float volume() const { return m_volume; }
    QString file() const { return m_file; }
    QJsonObject properties() const { return m_properties; }
    ContentTypes::InstalledType type() const { return m_type; }
    Video::FillMode fillMode() const { return m_fillMode; }
    QVector<int> monitors() const { return m_monitors; }
    QString title() const { return m_title; }

    void setIsLooping(bool value) { m_isLooping = value; }
    void setAbsolutePath(const QString& value) { m_absolutePath = value; }
    void setPreviewImage(const QString& value) { m_previewImage = value; }
    void setVolume(float value) { m_volume = value; }
    void setFile(const QString& value) { m_file = value; }
    void setProperties(const QJsonObject& value) { m_properties = value; }
    void setType(ContentTypes::InstalledType value) { m_type = value; }
    void setFillMode(Video::FillMode value) { m_fillMode = value; }
    void setMonitors(const QVector<int>& value) { m_monitors = value; }
    void setTitle(const QString& title) { m_title = title; }

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE bool hasContent();

    QJsonObject serialize() const;
    static std::optional<WallpaperData> loadTimelineWallpaperConfig(const QJsonObject& wallpaperObj);

private:
    bool m_isLooping = true;
    QString m_absolutePath;
    QString m_previewImage;
    QString m_title;
    float m_volume = 1.0f;
    QString m_file;
    QJsonObject m_properties;
    ContentTypes::InstalledType m_type = ContentTypes::InstalledType::Unknown;
    Video::FillMode m_fillMode = Video::FillMode::Fill;
    QVector<int> m_monitors {};
};
}
