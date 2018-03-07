#pragma once

#include <QObject>
#include <QString>

/*!
    \class Global Navigation Helper
    \brief Easy to use global object to use to navigate the main menu.

*/

class GlobalNavigationHelper : public QObject {
    Q_OBJECT

public:
    explicit GlobalNavigationHelper(QObject* parent = nullptr);

signals:
    void requestNavigation(QString nav);
    void requestToggleWallpaperConfiguration();

public slots:
    void setNavigation(QString nav);
    void setToggleWallpaperConfiguration();
};


