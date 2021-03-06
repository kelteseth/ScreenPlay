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
#include <QFileSystemWatcher>

#ifdef Q_OS_WIN
#include <QFileSystemWatcher>
#include <qt_windows.h>
#endif

#include "ScreenPlayUtil/util.h"
#include "screenplaysdk.h"
#include <memory>

class WidgetWindow : public QObject {
    Q_OBJECT

public:
    explicit WidgetWindow(
        const QString& projectPath,
        const QString& appid,
        const QString& type,
        const QPoint& position,
        const bool debugMode = false);

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString projectSourceFile READ projectSourceFile WRITE setProjectSourceFile NOTIFY projectSourceFileChanged)
    Q_PROPERTY(QUrl projectSourceFileAbsolute READ projectSourceFileAbsolute WRITE setProjectSourceFileAbsolute NOTIFY projectSourceFileAbsoluteChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(ScreenPlay::InstalledType::InstalledType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ScreenPlaySDK* sdk READ sdk WRITE setSdk NOTIFY sdkChanged)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)

    QString appID() const { return m_appID; }
    QPoint position() const { return m_position; }
    const QString& projectPath() const { return m_projectPath; }
    ScreenPlay::InstalledType::InstalledType type() const { return m_type; }
    const QString& projectSourceFile() const { return m_projectSourceFile; }
    const QUrl& projectSourceFileAbsolute() const { return m_projectSourceFileAbsolute; }
    ScreenPlaySDK* sdk() const { return m_sdk.get(); }
    bool debugMode() const { return m_debugMode; }

signals:
    void qmlExit();
    void reloadQML(const ScreenPlay::InstalledType::InstalledType oldType);
    void appIDChanged(QString appID);
    void qmlSceneValueReceived(QString key, QString value);
    void positionChanged(QPoint position);
    void projectPathChanged(const QString& projectPath);
    void typeChanged(ScreenPlay::InstalledType::InstalledType);
    void projectSourceFileChanged(const QString& projectSourceFile);
    void projectSourceFileAbsoluteChanged(const QUrl& projectSourceFileAbsolute);
    void sdkChanged(ScreenPlaySDK* sdk);
    void debugModeChanged(bool debugMode);

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

    void setProjectPath(const QString& projectPath)
    {
        if (m_projectPath == projectPath)
            return;
        m_projectPath = projectPath;
        emit projectPathChanged(m_projectPath);
    }

    void setType(ScreenPlay::InstalledType::InstalledType Type)
    {
        if (m_type == Type)
            return;
        m_type = Type;
        emit typeChanged(m_type);
    }

    void setProjectSourceFile(const QString& ProjectSourceFile)
    {
        if (m_projectSourceFile == ProjectSourceFile)
            return;
        m_projectSourceFile = ProjectSourceFile;
        emit projectSourceFileChanged(m_projectSourceFile);
    }

    void setProjectSourceFileAbsolute(const QUrl& projectSourceFileAbsolute)
    {
        if (m_projectSourceFileAbsolute == projectSourceFileAbsolute)
            return;
        m_projectSourceFileAbsolute = projectSourceFileAbsolute;
        emit projectSourceFileAbsoluteChanged(m_projectSourceFileAbsolute);
    }

    void setSdk(ScreenPlaySDK* sdk)
    {
        if (m_sdk.get() == sdk)
            return;
        m_sdk.reset(sdk);
        emit sdkChanged(sdk);
    }

    void setDebugMode(bool debugMode)
    {
        if (m_debugMode == debugMode)
            return;
        m_debugMode = debugMode;
        emit debugModeChanged(m_debugMode);
    }

private:
    void setupLiveReloading();

private:
    QString m_appID;
    QString m_projectPath;
    QString m_projectSourceFile;
    QJsonObject m_project;
    QPoint m_clickPos = { 0, 0 };
    QPoint m_lastPos = { 0, 0 };
    QPoint m_position = { 0, 0 };
    QQuickView m_window;
    std::unique_ptr<ScreenPlaySDK> m_sdk;
    QTimer m_positionMessageLimiter;
    ScreenPlay::InstalledType::InstalledType m_type;
    QFileSystemWatcher m_fileSystemWatcher;
    QTimer m_liveReloadLimiter;
    QUrl m_projectSourceFileAbsolute;
#ifdef Q_OS_WIN
    HWND m_hwnd;
#endif
    bool m_debugMode = false;
};
