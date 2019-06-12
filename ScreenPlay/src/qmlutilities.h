#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QtConcurrent/QtConcurrent>

namespace ScreenPlay {

class QMLUtilities : public QObject {
    Q_OBJECT

public:
    explicit QMLUtilities(QObject* parent = nullptr);

signals:
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();
    void setSidebarItem(QString screenId, QString type);

    void allLicenseLoaded(QString licensesText);
    void allDataProtectionLoaded(QString dataProtectionText);

public slots:

    void setNavigation(QString nav);
    void setNavigationActive(bool isActive);

    void setToggleWallpaperConfiguration();
    void openFolderInExplorer(QString url);

    void requestAllLicenses();
    void requestAllLDataProtection();

    QString fixWindowsPath(QString url);
};
}
