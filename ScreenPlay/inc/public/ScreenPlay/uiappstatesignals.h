#pragma once

#include <QObject>
#include <QQmlEngine>

namespace ScreenPlay {
class UiAppStateSignals : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    explicit UiAppStateSignals(QObject* parent = nullptr);
    Q_INVOKABLE void setNavigation(QString nav)
    {
        emit requestNavigation(nav);
    }

    // When we create a wallpaper the main navigation gets disabled
    Q_INVOKABLE void setNavigationActive(bool isActive)
    {
        emit requestNavigationActive(isActive);
    }

    Q_INVOKABLE void setToggleWallpaperConfiguration()
    {
        emit requestToggleWallpaperConfiguration();
    }

    Q_INVOKABLE void hideInstalledDrawer()
    {
        emit requestHideInstalledDrawer();
    }

signals:
    void requestToggleWallpaperConfiguration();
    void requestHideInstalledDrawer();
    void requestNavigation(QString nav);
    void requestNavigationActive(bool isActive);
};
}
