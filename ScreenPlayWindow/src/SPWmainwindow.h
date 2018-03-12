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
    explicit MainWindow(int i, QString projectPath, QString id, QScreen* parent = 0);
    ~MainWindow();
    QUrl projectPath() const;
    void setProjectPath(const QUrl& projectPath);
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)
    Q_PROPERTY(bool isVideo READ isVideo WRITE setIsVideo NOTIFY isVideoChanged)
    Q_PROPERTY(QString appID READ name WRITE setname NOTIFY nameChanged)

    QString projectConfig() const
    {
        return m_projectConfig;
    }

    bool isVideo() const
    {
        return m_isVideo;
    }


    QString name() const
    {
        return m_appID;
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

    void setIsVideo(bool isVideo)
    {
        if (m_isVideo == isVideo)
            return;

        m_isVideo = isVideo;
        emit isVideoChanged(m_isVideo);
    }



    void setname(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit nameChanged(m_appID);
    }

signals:
    void playVideo(QString path);
    void playQmlScene(QString file);
    void projectConfigChanged(QString projectConfig);
    void isVideoChanged(bool isVideo);


    void nameChanged(QString appID);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
    QUrl m_projectPath;
    QString m_projectFile;
    QJsonObject m_project;
    QString m_projectConfig;
    bool m_isVideo = false;

    QString m_appID;
};
