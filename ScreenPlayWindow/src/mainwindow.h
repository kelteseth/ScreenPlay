#pragma once

#include <QApplication>
#include <QEasingCurve>
#include <QObject>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QSharedPointer>
#include <QWindow>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>
#include <QJsonObject>

#include "qt_windows.h"

class MainWindow : public QWindow {
    Q_OBJECT

public:
    explicit MainWindow(int i,  QString projectPath, QScreen* parent = 0);
    void init();
    ~MainWindow();
    QUrl projectPath() const;
    void setProjectPath(const QUrl &projectPath);

public slots:
    void destroyThis();

signals:
    void playVideo(QString path);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
    QUrl m_projectPath;
    QString m_projectFile;
    QJsonObject m_project;

};
