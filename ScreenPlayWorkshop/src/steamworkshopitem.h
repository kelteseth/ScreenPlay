// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>

// Steam
#include "cstring"
#include "stdlib.h"
#include "steam/steam_api.h"
#include "steam/steam_qt_enums_generated.h"

#include "ScreenPlayUtil/util.h"
#include "steamapiwrapper.h"

namespace ScreenPlayWorkshop {

class SteamWorkshopItem : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(int uploadProgress READ uploadProgress WRITE setUploadProgress NOTIFY uploadProgressChanged)
    Q_PROPERTY(QUrl absolutePreviewImagePath READ absolutePreviewImagePath WRITE setAbsolutePreviewImagePath NOTIFY absolutePreviewImagePathChanged)
    Q_PROPERTY(ScreenPlayWorkshopSteamEnums::EResult status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QVariant publishedFileId READ publishedFileId WRITE setPublishedFileId NOTIFY publishedFileIdChanged)

public:
    SteamWorkshopItem(
        const QString& name,
        const QUrl& absolutePath,
        const quint64 appID);

    QString name() const { return m_name; }
    QUrl absolutePath() const { return m_absolutePath; }
    int uploadProgress() const { return m_uploadProgress; }
    QUrl absolutePreviewImagePath() const { return m_absolutePreviewImagePath; }
    ScreenPlayWorkshopSteamEnums::EResult status() const { return m_status; }
    QVariant publishedFileId() const { return m_publishedFileId; }

public slots:
    void createWorkshopItem();
    void checkUploadProgress();

    void setName(QString name)
    {
        if (m_name == name)
            return;

        m_name = name;
        emit nameChanged(m_name);
    }

    void setAbsolutePath(QUrl absolutePath)
    {
        if (m_absolutePath == absolutePath)
            return;

        m_absolutePath = absolutePath;
        emit absolutePathChanged(m_absolutePath);
    }

    void setUploadProgress(int uploadProgress)
    {

        if (m_uploadProgress == uploadProgress)
            return;

        m_uploadProgress = uploadProgress;
        emit uploadProgressChanged(m_uploadProgress);
    }

    void setAbsolutePreviewImagePath(QUrl absolutePreviewImagePath)
    {

        if (m_absolutePreviewImagePath == absolutePreviewImagePath)
            return;

        m_absolutePreviewImagePath = absolutePreviewImagePath;
        emit absolutePreviewImagePathChanged(m_absolutePreviewImagePath);
    }

    void setStatus(ScreenPlayWorkshopSteamEnums::EResult status)
    {
        if (m_status == status)
            return;

        m_status = status;
        emit statusChanged(m_status);
    }

    void setPublishedFileId(QVariant publishedFileId)
    {
        if (m_publishedFileId == publishedFileId)
            return;

        m_publishedFileId = publishedFileId;
        emit publishedFileIdChanged(m_publishedFileId);
    }

signals:
    void nameChanged(QString name);
    void absolutePathChanged(QUrl absolutePath);
    void uploadProgressChanged(float uploadProgress);
    void removeThis(SteamWorkshopItem* item);
    void absolutePreviewImagePathChanged(QUrl absolutePreviewImagePath);
    void uploadComplete(bool successful);
    void statusChanged(ScreenPlayWorkshopSteamEnums::EResult status);
    void uploadFailed(const quint64 m_PublishedFileId);
    void userNeedsToAcceptWorkshopLegalAgreement();
    void publishedFileIdChanged(QVariant publishedFileId);

private:
    CCallResult<SteamWorkshopItem, CreateItemResult_t> m_createWorkshopItemCallResult;
    void uploadItemToWorkshop(CreateItemResult_t* pCallback, bool bIOFailure);

    CCallResult<SteamWorkshopItem, SubmitItemUpdateResult_t> m_submitItemUpdateResultResult;
    void submitItemUpdateStatus(SubmitItemUpdateResult_t* pCallback, bool bIOFailure);

    void saveWorkshopID();

private:
    QString m_name;
    QUrl m_absolutePath;
    QUrl m_absolutePreviewImagePath;
    quint64 m_appID { 0 };
    const int m_updateTimerInterval { 500 };
    int m_uploadProgress { 0 }; // 0 - 100
    ScreenPlayWorkshopSteamEnums::EResult m_status { ScreenPlayWorkshopSteamEnums::EResult::K_EResultNone };
    UGCUpdateHandle_t m_UGCUpdateHandle { 0 };
    SubmitItemUpdateResult_t m_submitItemUpdateResultHanlde;
    QTimer m_updateTimer;
    QVariant m_publishedFileId { 0 };
};
}
