#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QRect>
#include <QWindow>
#include <qt_windows.h>

#include "profile.h"

class Wallpaper : public QWindow {
    Q_OBJECT
public:
    explicit Wallpaper(QWindow* parent = 0);
    Wallpaper(Profile profile);
    QQmlContext* context() const;


private:
    HWND m_hwnd = nullptr;
    HWND m_worker_hwnd = nullptr;
    QQuickView* m_quickRenderer = nullptr;
    QQmlContext* m_context = nullptr;

    Profile m_profile;
};

#endif // WALLPAPER_H
