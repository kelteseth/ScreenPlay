// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once


#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
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

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlayUtil/contenttypes.h"
#include "ScreenPlayUtil/util.h"

#include <fstream>
#include <iostream>
#include <optional>

namespace QArchive {
class DiskCompressor;
class DiskExtractor;
}

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

class Util : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString debugMessages READ debugMessages NOTIFY debugMessagesChanged)

public:
    Util();
    ~Util();

    QString debugMessages() const { return m_debugMessages; }

signals:
    void extractionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void extractionFinished();
    void compressionProgressChanged(QString file, int proc, int total, qint64 br, qint64 bt);
    void compressionFinished();

    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString folderName, ScreenPlay::InstalledType::InstalledType type);
    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);
    void debugMessagesChanged(QString debugMessages);

public slots:
    void copyToClipboard(const QString& text) const;
    void openFolderInExplorer(const QString& url) const;
    QString toLocal(const QString& url) const;
    bool exportProject(QString contentPath, QString exportFileName);
    bool importProject(QString archivePath, QString extractionPath);
    void requestAllLicenses();
    void requestDataProtection();
    bool fileExists(const QString& filePath) const;

    static void logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
    static bool writeSettings(const QJsonObject& obj, const QString& absolutePath);
    static bool writeFile(const QString& text, const QString& absolutePath);
    static bool writeFileFromQrc(const QString& qrcPath, const QString& absolutePath);
    static bool copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination);

    void setNavigation(QString nav)
    {
        emit requestNavigation(nav);
    }

    // When we create a wallpaper the main navigation gets disabled
    void setNavigationActive(bool isActive)
    {
        emit requestNavigationActive(isActive);
    }

    void setToggleWallpaperConfiguration()
    {
        emit requestToggleWallpaperConfiguration();
    }

    void appendDebugMessages(QString debugMessages)
    {
        if (m_debugMessages.size() > 10000) {
            m_debugMessages = "###### DEBUG CLEARED ######";
        }

        m_debugMessages += debugMessages;
        emit debugMessagesChanged(m_debugMessages);
    }

private:
    QString m_debugMessages {};
    QFuture<void> m_requestAllLicensesFuture;
    std::unique_ptr<QArchive::DiskCompressor> m_compressor;
    std::unique_ptr<QArchive::DiskExtractor> m_extractor;
};

// Used for redirect content from static logToGui to setDebugMessages
static Util* utilPointer { nullptr };
}
