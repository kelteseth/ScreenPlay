// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>
#include <QLocalSocket>
#include <QObject>
#include <QPoint>
#include <QProcess>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/sdkconnection.h"
#include "ScreenPlayUtil/util.h"

#include <memory>
#include <utility>

namespace ScreenPlay {

class ScreenPlayWidget : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(InstalledType::InstalledType type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit ScreenPlayWidget(
        const QString& appID,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QPoint& position,
        const QString& absolutePath,
        const QString& previewImage, const QJsonObject& properties,
        const InstalledType::InstalledType type);

    bool start();

    ScreenPlayWidget() { }

    QString previewImage() const { return m_previewImage; }
    QPoint position() const { return m_position; }
    QString absolutePath() const { return m_absolutePath; }
    QString appID() const { return m_appID; }
    InstalledType::InstalledType type() const { return m_type; }

    void setSDKConnection(std::unique_ptr<SDKConnection> connection);

    ProjectSettingsListModel* getProjectSettingsListModel() { return &m_projectSettingsListModel; }

public slots:
    void close();
    QJsonObject getActiveSettingsJson();

    void setPreviewImage(QString previewImage)
    {
        if (m_previewImage == previewImage)
            return;

        m_previewImage = previewImage;
        emit previewImageChanged(m_previewImage);
    }

    void setPosition(QPoint position)
    {
        if (m_position == position)
            return;

        m_position = position;
        emit positionChanged(m_position);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setType(InstalledType::InstalledType type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_absolutePath == absolutePath)
            return;

        m_absolutePath = absolutePath;
        emit absolutePathChanged(m_absolutePath);
    }

signals:
    void previewImageChanged(QString previewImage);
    void positionChanged(QPoint position);
    void appIDChanged(QString appID);
    void typeChanged(InstalledType::InstalledType type);
    void absolutePathChanged(QString absolutePath);

    void requestSave();
    void requestClose(const QString& appID);
    void error(const QString& msg);

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    ProjectSettingsListModel m_projectSettingsListModel;

    QProcess m_process;
    QString m_previewImage;
    QString m_appID;
    QPoint m_position;
    InstalledType::InstalledType m_type;
    QString m_absolutePath;
    QTimer m_pingAliveTimer;
    QStringList m_appArgumentsList;
};
}
