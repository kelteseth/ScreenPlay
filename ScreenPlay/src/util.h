/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include <QScopeGuard>
#include <QString>
#include <QTextStream>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrent>
#include <qqml.h>

#include <fstream>
#include <iostream>
#include <optional>

#include "globalvariables.h"
#include "libzippp.h"

namespace ScreenPlay {

template <typename T>
T QStringToEnum(const QString& key, const T defaultValue)
{
    auto metaEnum = QMetaEnum::fromType<T>();

    bool ok = false;
    T wantedEnum = static_cast<T>(metaEnum.keyToValue(key.toUtf8(), &ok));

    if (ok) {
        return wantedEnum;
    }

    return defaultValue;
}

class Util : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool ffmpegAvailable READ ffmpegAvailable NOTIFY ffmpegAvailableChanged)
    Q_PROPERTY(AquireFFMPEGStatus aquireFFMPEGStatus READ aquireFFMPEGStatus NOTIFY aquireFFMPEGStatusChanged)
    Q_PROPERTY(QString debugMessages READ debugMessages NOTIFY debugMessagesChanged)

public:
    explicit Util(QNetworkAccessManager* networkAccessManager, QObject* parent = nullptr);

    enum class AquireFFMPEGStatus {
        Init,
        Download,
        DownloadFailed,
        DownloadSuccessful,
        Extracting,
        ExtractingFailedReadFromBuffer,
        ExtractingFailedFFMPEG,
        ExtractingFailedFFMPEGSave,
        ExtractingFailedFFPROBE,
        ExtractingFailedFFPROBESave,
        ExtractingSuccessful,
        FinishedSuccessful,
    };
    Q_ENUM(AquireFFMPEGStatus)

    bool ffmpegAvailable() const
    {
        return m_ffmpegAvailable;
    }

    AquireFFMPEGStatus aquireFFMPEGStatus() const
    {
        return m_aquireFFMPEGStatus;
    }

    QString debugMessages() const
    {
        return m_debugMessages;
    }

signals:
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString folderName, ScreenPlay::InstalledType::InstalledType type);
    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);
    void ffmpegAvailableChanged(bool ffmpegAvailable);
    void aquireFFMPEGStatusChanged(ScreenPlay::Util::AquireFFMPEGStatus aquireFFMPEGStatus);
    void debugMessagesChanged(QString debugMessages);

public slots:
    void copyToClipboard(const QString& text) const;
    void openFolderInExplorer(const QString& url) const;

    void requestAllLicenses();
    void requestDataProtection();

    void downloadFFMPEG();

    static SearchType::SearchType getSearchTypeFromInstalledType(const InstalledType::InstalledType type);
    static std::optional<InstalledType::InstalledType> getInstalledTypeFromString(const QString& type);
    static std::optional<QJsonObject> parseQByteArrayToQJsonObject(const QByteArray& byteArray);
    static std::optional<QJsonObject> openJsonFileToObject(const QString& path);
    static std::optional<QString> openJsonFileToString(const QString& path);
    static std::optional<QVersionNumber> getVersionNumberFromString(const QString& str);
    static bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
    static QString toString(const QStringList& list);

    static void logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static QString generateRandomString(quint32 length = 32);

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

    void setFfmpegAvailable(bool ffmpegAvailable)
    {
        if (m_ffmpegAvailable == ffmpegAvailable)
            return;

        m_ffmpegAvailable = ffmpegAvailable;
        emit ffmpegAvailableChanged(m_ffmpegAvailable);
    }

    void setAquireFFMPEGStatus(ScreenPlay::Util::AquireFFMPEGStatus aquireFFMPEGStatus)
    {
        if (m_aquireFFMPEGStatus == aquireFFMPEGStatus)
            return;

        m_aquireFFMPEGStatus = aquireFFMPEGStatus;
        emit aquireFFMPEGStatusChanged(m_aquireFFMPEGStatus);
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
    bool saveExtractedByteArray(libzippp::ZipEntry& entry, std::string& absolutePathAndName);

private:
    QNetworkAccessManager* m_networkAccessManager { nullptr };

    bool m_ffmpegAvailable { false };
    AquireFFMPEGStatus m_aquireFFMPEGStatus { AquireFFMPEGStatus::Init };
    QString m_debugMessages {};
};

// Used for redirect content from static logToGui to setDebugMessages
static Util* utilPointer { nullptr };
}
