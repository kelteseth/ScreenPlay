// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QMetaType>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QScopeGuard>
#include <QString>
#include <QTextStream>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>
#include <optional>

#include "ScreenPlayUtil/contenttypes.h"
#include "qcorotask.h"
#include "qml/qcoroqml.h"
#include "qml/qcoroqmltask.h"

namespace ScreenPlay {

template <typename T>
T QStringToEnum(const QString& key, const T defaultValue)
{
    auto metaEnum = QMetaEnum::fromType<T>();

    bool ok = false;
    T wantedEnum = static_cast<T>(metaEnum.keyToValue(key.toUtf8(), &ok));

    if (ok) {
        return wantedEnum;
    } else {
        qWarning() << "Unable to convert QStringToEnum. Key: " << key;
    }

    return defaultValue;
}

class GodotExport : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum class Result {
        Failed,
        Ok,
    };
    Q_ENUM(Result)
};

class Result {
    Q_GADGET
    Q_PROPERTY(bool success READ success WRITE setSuccess)
    Q_PROPERTY(QVariant status READ status WRITE setStatus)
    Q_PROPERTY(QString message READ message WRITE setMessage)

public:
    explicit Result() { }
    explicit Result(bool success, const QVariant& status = {}, const QString& message = "")
    {
        m_success = success;
        m_status = status;
        m_message = message;
    }
    bool success() const { return m_success; }
    void setSuccess(bool success) { m_success = success; }
    QString message() const { return m_message; }
    void setMessage(const QString& message) { m_message = message; }
    QVariant status() const { return m_status; }
    void setStatus(const QVariant& status) { m_status = status; }

private:
    bool m_success;
    QString m_message;
    QVariant m_status;
};

class Util : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    QJsonArray fillArray(const QVector<QString>& items);
    ScreenPlay::ContentTypes::SearchType getSearchTypeFromInstalledType(const ScreenPlay::ContentTypes::InstalledType type);
    std::optional<ScreenPlay::ContentTypes::InstalledType> getInstalledTypeFromString(const QString& type);
    std::optional<ScreenPlay::Video::VideoCodec> getVideoCodecFromString(const QString& type);
    std::optional<QJsonObject> parseQByteArrayToQJsonObject(const QByteArray& byteArray);
    std::optional<QJsonObject> openJsonFileToObject(const QString& path);
    std::optional<QString> openJsonFileToString(const QString& path);
    std::optional<QVersionNumber> getVersionNumberFromString(const QString& str);
    bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
    bool writeSettings(const QJsonObject& obj, const QString& absolutePath);
    bool writeFile(const QString& text, const QString& absolutePath);
    bool writeFileFromQrc(const QString& qrcPath, const QString& absolutePath);
    bool copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination);
    QString toString(const QStringList& list) const;
    std::optional<QVector<int>> parseStringToIntegerList(const QString& string) const;
    float roundDecimalPlaces(const float number) const;
    QString generateRandomString(quint32 length = 32);
    QString executableAppEnding();
    QString executableBinEnding();
    QStringList getAvailableWallpaper() const;
    QStringList getAvailableWidgets() const;
    QStringList getAvailableTypes() const;
    QStringList getAvailableFillModes() const;
    // QML callable functions
    Q_INVOKABLE QString toLocal(const QString& url) const;
    Q_INVOKABLE QCoro::QmlTask exportGodotProject(const QString& absolutePath, const QString& godotEditorExecutablePath);

    Q_INVOKABLE bool isWallpaper(const ScreenPlay::ContentTypes::InstalledType type) const;
    Q_INVOKABLE bool isWidget(const ScreenPlay::ContentTypes::InstalledType type) const;
    Q_INVOKABLE bool isScene(const ScreenPlay::ContentTypes::InstalledType type) const;
    Q_INVOKABLE bool isVideo(const ScreenPlay::ContentTypes::InstalledType type) const;

    Q_INVOKABLE void copyToClipboard(const QString& text) const;
    Q_INVOKABLE void openFolderInExplorer(const QString& url) const;
    Q_INVOKABLE bool openGodotEditor(QString contentPath, QString godotEditorExecutablePath) const;
    Q_INVOKABLE void requestAllLicenses();
    Q_INVOKABLE void requestDataProtection();
    Q_INVOKABLE bool fileExists(const QString& filePath) const;

    Q_INVOKABLE void setNavigation(QString nav)
    {
        emit requestNavigation(nav);
    }

    // When we create a wallpaper the main navigation gets disabled
    Q_INVOKABLE void setNavigationActive(bool isActive)
    {
        emit requestNavigationActive(isActive);
    }

    Q_INVOKABLE void setToggleWallpaperConfiguration()
    {
        emit requestToggleWallpaperConfiguration();
    }

signals:
    void extractionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void extractionFinished();
    void compressionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void compressionFinished();

    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString folderName, ScreenPlay::ContentTypes::InstalledType type);
    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);

private:
    QFuture<void> m_requestAllLicensesFuture;
};
}
