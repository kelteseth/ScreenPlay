#pragma once

#include <QWindow>
#include <QScreen>
#include <QApplication>
#include <QSharedPointer>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QQuickView>


#include "qt_windows.h"

class MainWindow : public QWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QScreen *parent = 0);
    ~MainWindow();

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
};


