#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QWindow>
#include <qt_windows.h>

#include <QRect>

class Wallpaper : public QWindow {
    Q_OBJECT
public:
    explicit Wallpaper(QWindow* parent = 0);

    Wallpaper(QRect rect, float playback, bool isLooping, int fillmode);

    QQmlContext* context() const;

private:
    HWND hwnd = nullptr;
    HWND worker_hwnd = nullptr;
    QQuickView* quickRenderer = nullptr;
    QQmlContext* _context = nullptr;

    QRect m_rect;
    float m_playback;
    bool m_isLooping;
    int m_fillmode;
};

#endif // WALLPAPER_H
