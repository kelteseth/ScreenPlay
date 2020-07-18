/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QPoint>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QSharedPointer>
#include <QString>
#include <QWindow>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include <memory>

#include "screenplaysdk.h"

class WidgetWindow : public QObject {
    Q_OBJECT

public:
    explicit WidgetWindow(
        const QString& projectPath,
        const QPoint& position,
        const QString& appid,
        const QString& type);

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)
    Q_PROPERTY(QString sourcePath READ sourcePath WRITE setSourcePath NOTIFY sourcePathChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)

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

    QString sourcePath() const
    {
        return m_sourcePath;
    }

    QPoint position() const
    {
        return m_position;
    }

signals:
    void qmlExit();

    void appIDChanged(QString appID);
    void typeChanged(QString type);
    void projectConfigChanged(QString projectConfig);
    void sourcePathChanged(QString sourcePath);
    void qmlSceneValueReceived(QString key, QString value);

    void positionChanged(QPoint position);

public slots:
    void setSize(QSize size);
    void destroyThis();
    void messageReceived(QString key, QString value);
    void setPos(int xPos, int yPos);
    void setClickPos(const QPoint& clickPos);
    void setWidgetSize(const int with, const int height);
    void clearComponentCache();

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
    void setSourcePath(QString sourcePath)
    {
        if (m_sourcePath == sourcePath)
            return;

        m_sourcePath = sourcePath;
        emit sourcePathChanged(m_sourcePath);
    }

    QPointF cursorPos() { return QCursor::pos(); }
#ifdef Q_OS_WIN
    void setWindowBlur(unsigned int style = 3);
#endif

    void setPosition(QPoint position)
    {
        if (m_position == position)
            return;

        m_position = position;
        emit positionChanged(m_position);
    }

private:
    QString m_appID { "" };
    QString m_type { "qmlWidget" };
    QString m_projectConfig { "" };
    QString m_sourcePath { "" };

    QJsonObject m_project;
    QPoint m_clickPos = { 0, 0 };
    QPoint m_lastPos = { 0, 0 };
    QQuickView m_window;

#ifdef Q_OS_WIN
    HWND m_hwnd;
#endif
    QPoint m_position;
    std::unique_ptr<ScreenPlaySDK> m_sdk;
    QTimer m_positionMessageLimiter;
};
