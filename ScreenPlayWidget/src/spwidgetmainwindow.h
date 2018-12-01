#pragma once

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QPoint>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>
#include <QQuickWindow>
#include <QSharedPointer>
#include <QString>
#include <QWindow>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
typedef long HWND;
#endif

class SPWidgetmainwindow : public QWindow {
    Q_OBJECT

public:
    explicit SPWidgetmainwindow(QString projectPath, QString appid, QScreen* parent = nullptr);

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)

    QString appID() const
    {
        return m_appID;
    }

    QString type() const
    {
        return m_type;
    }

    QString projectConfig() const
    {
        return m_projectConfig;
    }

signals:
    void appIDChanged(QString appID);
    void typeChanged(QString type);
    void projectConfigChanged(QString projectConfig);
    void setWidgetSource(QString source);
    void qmlSceneValueReceived(QString key, QString value);

public slots:
    void setSize(QSize size);
    void destroyThis();
    void messageReceived(QString key, QString value);
    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }
    void setType(QString type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }
    void setProjectConfig(QString projectConfig)
    {
        if (m_projectConfig == projectConfig)
            return;

        m_projectConfig = projectConfig;
        emit projectConfigChanged(m_projectConfig);
    }

    void setPos(int xPos, int yPos);
    void setClickPos(const QPoint& clickPos);
    void SetWindowBlur(HWND hWnd);

private:
    QString m_appID;
    QString m_type = "qmlWidget";
    QString m_projectConfig;
    QJsonObject m_project;
    HWND m_hwnd;
    QPoint m_clickPos = { 0, 0 };

    QSharedPointer<QQuickView> m_quickRenderer;
};
