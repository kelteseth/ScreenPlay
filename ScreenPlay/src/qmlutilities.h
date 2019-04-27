#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QString>

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

public slots:
    void setNavigation(QString nav);
    void setNavigationActive(bool isActive);

    void setToggleWallpaperConfiguration();
    void openFolderInExplorer(QString url);

    QString fixWindowsPath(QString url);
};
