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

#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>
#include <QLocalSocket>
#include <QObject>
#include <QPoint>
#include <QProcess>

#include <memory>

#include "globalvariables.h"
#include "projectsettingslistmodel.h"
#include "sdkconnection.h"

namespace ScreenPlay {

class ScreenPlayWidget : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(InstalledType::InstalledType type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit ScreenPlayWidget(const QString& appID,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QPoint& position,
        const QString& absolutePath,
        const QString& previewImage, const QJsonObject& properties,
        const InstalledType::InstalledType type);

    ScreenPlayWidget() { }

    QString previewImage() const
    {
        return m_previewImage;
    }

    QPoint position() const
    {
        return m_position;
    }

    QString absolutePath() const
    {
        return m_absolutePath;
    }

    QString appID() const
    {
        return m_appID;
    }

    InstalledType::InstalledType type() const
    {
        return m_type;
    }

    void setSDKConnection(const std::shared_ptr<SDKConnection>& connection);

    ProjectSettingsListModel* getProjectSettingsListModel()
    {
        return &m_projectSettingsListModel;
    }

public slots:
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

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<SDKConnection> m_connection;
    ProjectSettingsListModel m_projectSettingsListModel;

    QProcess m_process;
    QString m_previewImage;
    QString m_appID;
    QPoint m_position;
    InstalledType::InstalledType m_type;
    QString m_absolutePath;
    QTimer m_pingAliveTimer;
};
}
