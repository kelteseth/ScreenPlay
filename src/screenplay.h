#ifndef SCREENPLAY_H
#define SCREENPLAY_H

#include <QQuickView>
#include <QWindow>
#include <qt_windows.h>


class ScreenPlay : public QWindow {
    Q_OBJECT
public:
    explicit ScreenPlay(QWindow* parent = 0);
    ScreenPlay(int width, int height);
    ~ScreenPlay();

signals:

public slots:

private:
    HWND hwnd = nullptr;
    HWND worker_hwnd = nullptr;
    QQuickView* quickRenderer = nullptr;
};


#endif // SCREENPLAY_H

