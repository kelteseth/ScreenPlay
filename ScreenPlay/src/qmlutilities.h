#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QtConcurrent/QtConcurrent>

/*!
    \class Global QML Utilities
    \brief Easy to use global object to use to:
    \list
        \i Navigate the main menu
        \i Open Explorer at a given path
    \endlist
*/

class QMLUtilities : public QObject {
    Q_OBJECT
public:
    explicit QMLUtilities(QObject* parent = nullptr);

signals:
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
    void requestToggleWallpaperConfiguration();

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
