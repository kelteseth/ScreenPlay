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
#include <fstream>
#include <iostream>

#include "libzippp/libzippp.h"

namespace ScreenPlay {

class QMLUtilities : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool ffmpegAvailable READ ffmpegAvailable WRITE setFfmpegAvailable NOTIFY ffmpegAvailableChanged)

public:
    explicit QMLUtilities(QNetworkAccessManager* networkAccessManager, QObject* parent = nullptr);

    bool ffmpegAvailable() const
    {
        return m_ffmpegAvailable;
    }

signals:
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString screenId, QString type);

    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);

    void downloadFFMPEGCompleted(bool successful);

    void ffmpegAvailableChanged(bool ffmpegAvailable);

public slots:

    void setNavigation(QString nav);
    void setNavigationActive(bool isActive);

    void setToggleWallpaperConfiguration();
    void openFolderInExplorer(QString url);

    void requestAllLicenses();
    void requestAllLDataProtection();

    void downloadFFMPEG();

    QString fixWindowsPath(QString url);

    void setFfmpegAvailable(bool ffmpegAvailable)
    {
        if (m_ffmpegAvailable == ffmpegAvailable)
            return;

        m_ffmpegAvailable = ffmpegAvailable;
        emit ffmpegAvailableChanged(m_ffmpegAvailable);
    }

private:
    QNetworkAccessManager* m_networkAccessManager;
    bool m_ffmpegAvailable { false };

    bool saveExtractedByteArray(libzippp::ZipEntry& entry, std::string& absolutePathAndName);
};
}
