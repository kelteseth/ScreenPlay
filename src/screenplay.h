#ifndef SCREENPLAY_H
#define SCREENPLAY_H

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QWindow>
#include <qt_windows.h>

class ScreenPlay : public QWindow {
    Q_OBJECT
public:
    explicit ScreenPlay(QWindow* parent = 0);
    ScreenPlay(int width, int height);
    ~ScreenPlay();

    void loadQQuickView(QUrl path);
    void showQQuickView(int width, int height);

    QQmlContext* context() const;

signals:

public slots:
    void setVisible(bool visible);

private:
    HWND hwnd = nullptr;
    HWND worker_hwnd = nullptr;
    QQuickView* quickRenderer = nullptr;
    QQmlContext* _context = nullptr;
};

#endif // SCREENPLAY_H
