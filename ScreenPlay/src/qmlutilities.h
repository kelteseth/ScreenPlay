#pragma once

#include <QObject>
#include <QString>

/*!
    \class Global QML Utilities
    \brief Easy to use global object to use to:
    \list
        \i Navigate the main menu
        \i Open Explorer at a given path
    \endlist
*/

class QMLUtilities : public QObject
{
    Q_OBJECT
public:
    explicit QMLUtilities(QObject *parent = nullptr);

signals:
    void requestNavigation(QString nav);
    void requestToggleWallpaperConfiguration();

public slots:
    void setNavigation(QString nav);
    void setToggleWallpaperConfiguration();
};
