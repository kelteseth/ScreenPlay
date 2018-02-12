#pragma once

#include <QApplication>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QSharedPointer>
#include <QWindow>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

#include "qt_windows.h"

class MainWindow : public QWindow {
    Q_OBJECT

public:
    explicit MainWindow(QScreen* parent = 0);
    void init();
    ~MainWindow();
public slots:
    void destroyThis();

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
};
