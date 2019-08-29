#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QObject>
#include <QProcess>
#include <QScopeGuard>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <QDateTime>
#include <QNetworkReply>
#include <QProcess>
#include <qqml.h>

#include <fstream>
#include <iostream>
#include <optional>

#include "libzippp/libzippp.h"

namespace ScreenPlay {




class Util : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool ffmpegAvailable READ ffmpegAvailable  NOTIFY ffmpegAvailableChanged)
    Q_PROPERTY(AquireFFMPEGStatus aquireFFMPEGStatus READ aquireFFMPEGStatus NOTIFY aquireFFMPEGStatusChanged)
    Q_PROPERTY(QString debugMessages READ debugMessages  NOTIFY debugMessagesChanged)

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
    void setSidebarItem(QString screenId, QString type);
    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);
    void ffmpegAvailableChanged(bool ffmpegAvailable);
    void aquireFFMPEGStatusChanged(AquireFFMPEGStatus aquireFFMPEGStatus);
    void debugMessagesChanged(QString debugMessages);

public slots:
    static std::optional<QJsonObject> openJsonFileToObject(const QString& path);
    static std::optional<QString> openJsonFileToString(const QString& path);
    static QString generateRandomString(quint32 length = 32);

    void setNavigation(QString nav);
    void setNavigationActive(bool isActive);

    void setToggleWallpaperConfiguration();
    void openFolderInExplorer(QString url);

    void requestAllLicenses();
    void requestAllLDataProtection();

    void downloadFFMPEG();

    static void logToGui(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    QString fixWindowsPath(QString url);

    void setFfmpegAvailable(bool ffmpegAvailable)
    {
        if (m_ffmpegAvailable == ffmpegAvailable)
            return;

        m_ffmpegAvailable = ffmpegAvailable;
        emit ffmpegAvailableChanged(m_ffmpegAvailable);
    }

    void setAquireFFMPEGStatus(AquireFFMPEGStatus aquireFFMPEGStatus)
    {
        if (m_aquireFFMPEGStatus == aquireFFMPEGStatus)
            return;

        m_aquireFFMPEGStatus = aquireFFMPEGStatus;
        emit aquireFFMPEGStatusChanged(m_aquireFFMPEGStatus);
    }

    void appendDebugMessages(QString debugMessages)
    {
        if(m_debugMessages.size() > 1000000) {
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
static Util* utilPointer {nullptr};
}