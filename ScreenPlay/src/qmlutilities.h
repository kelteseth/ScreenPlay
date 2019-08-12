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

#include <QNetworkReply>
#include <QProcess>
#include <qqml.h>

#include <fstream>
#include <iostream>

#include "libzippp/libzippp.h"

namespace ScreenPlay {

class QMLUtilities : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool ffmpegAvailable READ ffmpegAvailable WRITE setFfmpegAvailable NOTIFY ffmpegAvailableChanged)
    Q_PROPERTY(AquireFFMPEGStatus aquireFFMPEGStatus READ aquireFFMPEGStatus WRITE setAquireFFMPEGStatus NOTIFY aquireFFMPEGStatusChanged)
    Q_PROPERTY(QString debugMessages READ debugMessages WRITE setDebugMessages NOTIFY debugMessagesChanged)

public:
    explicit QMLUtilities(QNetworkAccessManager* networkAccessManager, QObject* parent = nullptr);

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

    void setNavigation(QString nav);
    void setNavigationActive(bool isActive);

    void setToggleWallpaperConfiguration();
    void openFolderInExplorer(QString url);

    void requestAllLicenses();
    void requestAllLDataProtection();

    void downloadFFMPEG();


    void redirectMessageOutputToMainWindow(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {


        QByteArray localMsg = msg.toLocal8Bit();
        QByteArray file = "File: " + QByteArray(context.file) + ", ";
        QByteArray line = "in line " + QByteArray::number(context.line) + ", ";
        //QByteArray function = "function " + QByteArray(context.function) + ", Message: ";

        //localMsg = file + line +  localMsg;
        //global_sdkPtr->redirectMessage(localMsg);

        switch (type) {
        case QtDebugMsg:
            //localMsg = " SDK START: " /*+  QByteArray::fromStdString(global_sdkPtr->contentType().toStdString()) + " "*/ + localMsg;
            break;
        case QtInfoMsg:
            //fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            //fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            //fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            //(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        }
    }


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

    void setDebugMessages(QString debugMessages)
    {
        if (m_debugMessages == debugMessages)
            return;

        m_debugMessages = debugMessages;
        emit debugMessagesChanged(m_debugMessages);
    }

private:
    QNetworkAccessManager* m_networkAccessManager;
    bool m_ffmpegAvailable { false };

    bool saveExtractedByteArray(libzippp::ZipEntry& entry, std::string& absolutePathAndName);
    AquireFFMPEGStatus m_aquireFFMPEGStatus { AquireFFMPEGStatus::Init };
    QString m_debugMessages;
};
}
