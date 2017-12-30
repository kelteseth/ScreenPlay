#include "steamworkshop.h"

SteamWorkshop::SteamWorkshop(QObject* parent)
    : QObject(parent)
{
}

SteamWorkshop::SteamWorkshop(AppId_t nConsumerAppId, SteamWorkshopListModel* wlm, Settings* s)
{
    m_AppId = nConsumerAppId;
    m_workshopListModel = wlm;
    m_settings = s;

    // Register namespace seperated enums because qml has no enum scope WTF
    qRegisterMetaType<LocalWorkshopCreationStatus::Value>();
    qRegisterMetaType<RemoteWorkshopCreationStatus::Value>();
    qmlRegisterUncreatableMetaObject(LocalWorkshopCreationStatus::staticMetaObject, "LocalWorkshopCreationStatus", 1, 0, "LocalWorkshopCreationStatus", "Error: only enums");
    qmlRegisterUncreatableMetaObject(RemoteWorkshopCreationStatus::staticMetaObject, "RemoteWorkshopCreationStatus", 1, 0, "RemoteWorkshopCreationStatus", "Error: only enums");
}

void SteamWorkshop::createWorkshopItem()
{
    SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(m_AppId, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
    m_createWorkshopItemCallResult.Set(hSteamAPICall, this, &SteamWorkshop::workshopItemCreated);
}

void SteamWorkshop::workshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure)
        return;

    m_UGCUpdateHandle = SteamUGC()->StartItemUpdate(m_AppId, pCallback->m_nPublishedFileId);
    emit workshopItemCreated(pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement, pCallback->m_eResult, pCallback->m_nPublishedFileId);
}

void SteamWorkshop::submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, const QUrl projectFile, const QUrl videoFile)
{

    // Ether way one of the url must have a value
    if (videoFile.isEmpty() && projectFile.isEmpty()) {
        emit remoteWorkshopCreationStatusChanged(RemoteWorkshopCreationStatus::ErrorUnknown);
        return;
    }

    QString absoluteContentPath;
    QFile projectConfig;

    QFileInfo tmp;
    tmp.setFile(projectFile.toString());
    absoluteContentPath = tmp.path();
    absoluteContentPath = absoluteContentPath.replace("file:///", "");
    absoluteContentPath += "/";
    projectConfig.setFileName(absoluteContentPath + "/project.json");

    QJsonObject jsonObject;
    QJsonDocument jsonProject;
    QJsonParseError parseError;

    projectConfig.open(QIODevice::ReadOnly | QIODevice::Text);
    QString projectConfigData = projectConfig.readAll();
    jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        emit remoteWorkshopCreationStatusChanged(RemoteWorkshopCreationStatus::ErrorUnknown);
        return;
    }

    jsonObject = jsonProject.object();
    QString preview = absoluteContentPath + jsonObject.value("preview").toString();
    //absoluteContentPath = absoluteContentPath.replace("/", "\\\\");

    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toLatin1()).data());
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toLatin1()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle, QByteArray(language.toLatin1()).data());
    SteamUGC()->SetItemContent(m_UGCUpdateHandle, QByteArray(absoluteContentPath.toLatin1()).data());
    SteamUGC()->SetItemPreview(m_UGCUpdateHandle, QByteArray(preview.toLatin1()).data());

    auto visibility = static_cast<ERemoteStoragePublishedFileVisibility>(remoteStoragePublishedFileVisibility);
    SteamUGC()->SetItemVisibility(m_UGCUpdateHandle, visibility);
    SteamUGC()->SubmitItemUpdate(m_UGCUpdateHandle, nullptr);

    emit remoteWorkshopCreationStatusChanged(RemoteWorkshopCreationStatus::Started);
}

int SteamWorkshop::getItemUpdateProcess()
{
    unsigned long long _itemProcessed = 0;
    unsigned long long _bytesTotoal = 0;
    EItemUpdateStatus status = SteamUGC()->GetItemUpdateProgress(m_UGCUpdateHandle, &_itemProcessed, &_bytesTotoal);
    qDebug() << _itemProcessed << _bytesTotoal << status;
    setItemProcessed(_itemProcessed);
    setBytesTotal(_bytesTotoal);
    return status;
}

void SteamWorkshop::getAPICallInfo()
{
    qDebug() << SteamUtils()->GetAPICallFailureReason(m_searchCall);
}

bool SteamWorkshop::contentFolderExist(QString folder)
{
    QString path = m_settings->localStoragePath().toString() + folder;

    if (QDir(path).exists()) {
        return true;
    } else {
        return false;
    }
}

void SteamWorkshop::createLocalWorkshopItem(QString title, QUrl videoPath, QUrl previewPath)
{
    QtConcurrent::run([=]() {

        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::Started);

        QString fromVideoPath = QString(videoPath.toString()).replace("file:///", "");
        QString fromImagePath = QString(previewPath.toString()).replace("file:///", "");
        QString toPath = m_settings->localStoragePath().toString() + "/" + title;
        QString toPathWithVideoFile = toPath + "/" + videoPath.fileName();
        QString toPathWithImageFile = toPath + "/" + previewPath.fileName();

        if (QDir(toPath).exists()) {
            if (!QDir(toPath).isEmpty()) {
                emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorFolder);
                return;
            } else {
                //if(!QDir(toPath + ))
            }

        } else {
            //TODO: Display Error
            if (!QDir().mkdir(toPath)) {
                emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorFolderCreation);
                return;
            }
        }

        //Copy Video File
        if (QFile::copy(fromVideoPath, toPathWithVideoFile)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::CopyVideoFinished);
        } else {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyVideo);
        }

        //Copy Image File
        if (QFile::copy(fromImagePath, toPathWithImageFile)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::CopyImageFinished);
        } else {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyImage);
        }

        //Copy Project File
        QFile configFile(toPath + "/" + "project.json");

        if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyConfig);
            return;
        }

        QTextStream out(&configFile);
        QJsonObject configObj;

        configObj.insert("file", videoPath.fileName());
        configObj.insert("preview", previewPath.fileName());
        //TODO
        configObj.insert("description", "");
        configObj.insert("title", title);

        QJsonDocument configJsonDocument(configObj);
        out << configJsonDocument.toJson();
        configFile.close();

        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::Finished);
    });
}

void SteamWorkshop::subscribeItem(unsigned int id)
{
    SteamUGC()->SubscribeItem(static_cast<unsigned long long>(id));
}

void SteamWorkshop::searchWorkshop()
{
    m_UGCSearchHandle = SteamUGC()->CreateQueryAllUGCRequest(EUGCQuery::k_EUGCQuery_RankedByVote,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_AppId,
        m_AppId,
        1);
    SteamUGC()->SetAllowCachedResponse(m_UGCSearchHandle, 3000);
    //Important: First send the request to get the Steam API Call
    //           then set the handler
    m_searchCall = SteamUGC()->SendQueryUGCRequest(m_UGCSearchHandle);
    m_steamUGCQueryResult.Set(m_searchCall, this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{

    if (bIOFailure)
        return;

    QtConcurrent::run([this, pCallback]() {    });

        SteamUGCDetails_t details;
        const int urlLength = 200;
        char url[urlLength];
        uint32 previews = 0;
        uint32 subscriber = 0;
        uint32 results = pCallback->m_unTotalMatchingResults;
        //qDebug() << results;

        for (uint32 i = 0; i < results; i++) {
            if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
                //qDebug() << "ok " << pCallback;
                if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url, static_cast<uint32>(urlLength))) {
                    QByteArray urlData(url);

                    //Todo use multiple preview for gif hover effect
                    previews = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);
                    if (i == 0) {
                        m_workshopListModel->setBannerWorkshopItem(details.m_nPublishedFileId, QString(details.m_rgchTitle), QUrl(urlData),0);
                        emit workshopSearched();
                    } else {
                        emit workshopSearchResult(details.m_nPublishedFileId, QString(details.m_rgchTitle), QUrl(urlData),0);
                    }
//                    if(SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle,i,EItemStatistic::k_EItemStatistic_NumSubscriptions,&subscriber)){

//                    }
                }
            }
        }

        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);

}
