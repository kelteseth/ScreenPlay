#pragma once

#include <QWindow>
#include <QSharedPointer>
#include <QQuickView>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlContext>


class SPWidgetmainwindow : public QWindow {
    Q_OBJECT

public:
    explicit SPWidgetmainwindow( QString projectPath, QString appid,QScreen* parent = nullptr);

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

public slots:
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

private:
    QString m_appID;
    QString m_type = "qmlWidget";
    QString m_projectConfig;
    QJsonObject m_project;

    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
};
