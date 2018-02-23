#pragma once

#include "qt_windows.h"
#include <QApplication>
#include <QDir>
#include <QEasingCurve>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QSharedPointer>
#include <QWindow>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

class MainWindow : public QWindow {
    Q_OBJECT

public:
    explicit MainWindow(int i, QString projectPath, QScreen* parent = 0);
    ~MainWindow();
    QUrl projectPath() const;
    void setProjectPath(const QUrl& projectPath);
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)

    QString projectConfig() const
    {
        return m_projectConfig;
    }

public slots:
    void destroyThis();
    void init();

    void setProjectConfig(QString projectConfig)
    {
        if (m_projectConfig == projectConfig)
            return;

        m_projectConfig = projectConfig;
        emit projectConfigChanged(m_projectConfig);
    }

signals:
    void playVideo(QString path);
    void playQmlScene(QString file);
    void projectConfigChanged(QString projectConfig);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
    QUrl m_projectPath;
    QString m_projectFile;
    QJsonObject m_project;
    QString m_projectConfig;
};
