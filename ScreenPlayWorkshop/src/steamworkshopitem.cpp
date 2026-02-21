// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshopitem.h"

#include <QStringList>
namespace ScreenPlayWorkshop {

SteamWorkshopItem::SteamWorkshopItem(const QString& name, const QUrl& absolutePath, const quint64 appID)
    : QObject()
    , m_name { name }
    , m_absolutePath { absolutePath }
    , m_uploadProgress { 0 }
    , m_appID { appID }
{
    QObject::connect(&m_updateTimer, &QTimer::timeout, this, &SteamWorkshopItem::checkUploadProgress);
}

void SteamWorkshopItem::createWorkshopItem()
{
    SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(m_appID, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
    SteamAsyncCall<CreateItemResult_t>::create(
        hSteamAPICall,
        [this](CreateItemResult_t* cb, bool io) { uploadItemToWorkshop(cb, io); },
        this);
}

void SteamWorkshopItem::checkUploadProgress()
{
    quint64 _itemProcessed = 0;
    quint64 _bytesTotoal = 0;
    EItemUpdateStatus status = SteamUGC()->GetItemUpdateProgress(m_UGCUpdateHandle, &_itemProcessed, &_bytesTotoal);

    const auto newState = static_cast<ScreenPlayCore::Steam::EItemUpdateStatus>(status);
    const bool stateChanged = (newState != m_uploadState);

    setUploadState(newState);

    // Only log when state changes to reduce console spam
    if (stateChanged) {
        qInfo() << name() << "state:" << status << "progress:" << uploadProgress() << "%";
    }

    // Calculate progress from bytes if available
    if (_bytesTotoal > 0) {
        float progress = static_cast<float>(_itemProcessed) / static_cast<float>(_bytesTotoal);
        // Clamp to valid range and convert to percentage
        if (progress >= 0.0f && progress <= 1.0f) {
            setUploadProgress(static_cast<int>(progress * 100));
        }
    }
}
void SteamWorkshopItem::uploadItemToWorkshop(CreateItemResult_t* pCallback, bool bIOFailure)
{
    if (!pCallback) {
        qWarning() << "Invalid CreateItemResult_t pointer";
        emit removeThis(this);
        return;
    }

    if (pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement) {
        emit userNeedsToAcceptWorkshopLegalAgreement();
    }

    if (bIOFailure) {
        qWarning() << "workshopItemCreated IO Failure";
        emit removeThis(this);
        return;
    }

    ScreenPlay::Util util;
    const QString absoluteContentPath = util.toLocal(m_absolutePath.toString());
    auto jsonObjectOpt = util.openJsonFileToObject(absoluteContentPath + "/project.json");

    if (!jsonObjectOpt.has_value()) {
        qWarning() << "Unable to load project file";
        emit removeThis(this);
        return;
    }

    auto jsonObject = jsonObjectOpt.value();
    QString preview = absoluteContentPath + "/" + jsonObject.value("preview").toString();
    setAbsolutePreviewImagePath(preview);

    // Only now we have an workshop id, so we have to add this for later use
    if (!jsonObject.contains("workshopid")) {
        jsonObject.insert("workshopid", QJsonValue::fromVariant(pCallback->m_nPublishedFileId));
    }

    QString title;
    if (jsonObject.contains("title")) {
        title = jsonObject.value("title").toString();
        setName(title);
    }

    QString description;
    if (jsonObject.contains("description")) {
        description = jsonObject.value("description").toString();
    }

    QString language;
    if (jsonObject.contains("language")) {
        language = jsonObject.value("language").toString();
    }

    QString youtube;
    if (jsonObject.contains("youtube")) {
        youtube = jsonObject.value("youtube").toString();
    }

    if (!youtube.isEmpty()) {
        SteamUGC()->AddItemPreviewVideo(m_UGCUpdateHandle, QByteArray(youtube.toUtf8().data()));
    }

    QDir absoluteContentdir { absoluteContentPath };

    if (jsonObject.contains("previewGIF")) {
        const QString previewGIF = jsonObject.value("previewGIF").toString();
        QFile previewGifFile { absoluteContentdir.path() + "/" + previewGIF };
        qInfo() << previewGifFile.size();
        if (previewGifFile.exists() && previewGifFile.size() <= (1000 * 1000))
            SteamUGC()->AddItemPreviewFile(m_UGCUpdateHandle,
                QByteArray(QString { absoluteContentPath + "/" + previewGIF }.toUtf8()).data(),
                EItemPreviewType::k_EItemPreviewType_Image);
    }

    if (absoluteContentdir.exists("previewWEBM")) {
        const QString previewWEBM = jsonObject.value("previewWEBM").toString();
        QFile previewWEBMFile { absoluteContentdir.path() + "/" + previewWEBM };
        qInfo() << previewWEBMFile.size();
        if (previewWEBMFile.exists() && previewWEBMFile.size() <= (1000 * 1000))
            SteamUGC()->AddItemPreviewFile(m_UGCUpdateHandle,
                QByteArray(QString { absoluteContentPath + "/preview.webm" }.toUtf8()).data(),
                EItemPreviewType::k_EItemPreviewType_Image);
    }

    QStringList tags;
    if (jsonObject.contains("tags")) {
        QJsonArray tagArray = jsonObject.value("tags").toArray();
        for (const QJsonValue& item : tagArray) {
            tags.append(item.toString());
        }
    }

    // Add the type of the project as tag
    if (jsonObject.contains("type")) {
        tags.append(jsonObject.value("type").toString());
    }

    m_UGCUpdateHandle = SteamUGC()->StartItemUpdate(m_appID, pCallback->m_nPublishedFileId);
    if (m_UGCUpdateHandle == k_UGCUpdateHandleInvalid) {
        qWarning() << "StartItemUpdate returned invalid handle - cannot upload item";
        setStatus(ScreenPlayCore::Steam::EResult::K_EResultFail);
        emit uploadComplete(false);
        return;
    }

    SteamTagArray tagArray(tags);
    const bool success = SteamUGC()->SetItemTags(m_UGCUpdateHandle, tagArray.get());
    if (!success) {
        qWarning() << "Failed to set item tags";
    }
    SteamUGC()->AddItemPreviewFile(m_UGCUpdateHandle, QByteArray(preview.toUtf8()).data(), EItemPreviewType::k_EItemPreviewType_Image);
    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toUtf8().data()));
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toUtf8()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle, QByteArray(language.toUtf8()).data());
    SteamUGC()->SetItemContent(m_UGCUpdateHandle, QByteArray(absoluteContentPath.toUtf8()).data());
    SteamUGC()->SetItemPreview(m_UGCUpdateHandle, QByteArray(preview.toUtf8()).data());
    SteamUGC()->SetItemVisibility(m_UGCUpdateHandle, ERemoteStoragePublishedFileVisibility::k_ERemoteStoragePublishedFileVisibilityPublic);

    m_publishedFileId = QVariant::fromValue<uint64>(pCallback->m_nPublishedFileId);
    saveWorkshopID();

    SteamAPICall_t apicall = SteamUGC()->SubmitItemUpdate(m_UGCUpdateHandle, nullptr);
    if (apicall == k_uAPICallInvalid) {
        qWarning() << "Failed to submit item update";
        return;
    }

    SteamAsyncCall<SubmitItemUpdateResult_t>::create(
        apicall,
        [this](SubmitItemUpdateResult_t* cb, bool io) { submitItemUpdateStatus(cb, io); },
        this);
    m_updateTimer.start(m_updateTimerInterval);
}

void SteamWorkshopItem::submitItemUpdateStatus(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        qWarning() << "submitItemUpdateStatus bIOFailure";
        return;
    }

    qDebug() << pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement << pCallback->m_eResult << pCallback->m_nPublishedFileId;

    if (pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement)
        emit userNeedsToAcceptWorkshopLegalAgreement();

    setStatus(static_cast<ScreenPlayCore::Steam::EResult>(pCallback->m_eResult));

    switch (pCallback->m_eResult) {
    case EResult::k_EResultOK: {
        emit uploadComplete(true);
        setUploadProgress(100);
        break;
    }
    case EResult::k_EResultFail: {

        emit uploadComplete(false);
        setUploadProgress(0);
        break;
    }
    default: { 
        // Intermediate/unexpected states — log but do not treat as completed
        qDebug() << "Unexpected submit result:" << pCallback->m_eResult;
        break;
    }
    }

    m_updateTimer.stop();
}

void SteamWorkshopItem::saveWorkshopID()
{
    ScreenPlay::Util util;
    const QString path = QUrl::fromUserInput(m_absolutePath.toString() + "/project.json").toLocalFile();
    qInfo() << m_absolutePath << m_publishedFileId << path;
    auto jsonProject = util.openJsonFileToObject(path);

    if (!jsonProject.has_value()) {
        qWarning() << "Could not parse project file!";
        return;
    }

    auto jsonObject = jsonProject.value();
    jsonObject.insert("workshopid", m_publishedFileId.toString());

    qInfo() << "Writing workshopID: " << m_publishedFileId.toString() << "into: " << path;
    if (!util.writeJsonObjectToFile(path, jsonObject)) {
        qWarning() << "Could not write project file!";
    }
}

}

#include "moc_steamworkshopitem.cpp"
