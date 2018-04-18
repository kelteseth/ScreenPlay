#include "steamworkshop.h"

SteamWorkshop::SteamWorkshop(QObject* parent)
    : QObject(parent)
{
}

void SteamWorkshop::initSteam()
{
    if (m_settings->offlineMode()) {

        if (SteamAPI_RestartAppIfNecessary(m_appID)) {
            qWarning() << "SteamAPI_RestartAppIfNecessary";
            m_steamErrorRestart = true;
        }

        if (!SteamAPI_Init()) {
            qWarning() << "Could not init steam sdk!";
            m_steamErrorAPIInit = true;
        }

        if (!(m_steamErrorAPIInit || m_steamErrorRestart)) {
            m_settings->setOfflineMode(false);
            m_pollTimer = new QTimer(this);
            QObject::connect(m_pollTimer, &QTimer::timeout, [&]() { SteamAPI_RunCallbacks(); });
            m_pollTimer->setInterval(250);
            m_pollTimer->start();
        }
    }
}

SteamWorkshop::SteamWorkshop(AppId_t nConsumerAppId, SteamWorkshopListModel* wlm, Settings* s)
{

    m_appID = nConsumerAppId;
    m_workshopListModel = wlm;
    m_settings = s;

    // Register namespace seperated enums because qml has no enum scope WTF

    qRegisterMetaType<RemoteWorkshopCreationStatus::Value>();
    qmlRegisterUncreatableMetaObject(RemoteWorkshopCreationStatus::staticMetaObject, "RemoteWorkshopCreationStatus", 1, 0, "RemoteWorkshopCreationStatus", "Error: only enums");

    initSteam();
}

void SteamWorkshop::createWorkshopItem()
{
    SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(m_appID, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
    m_createWorkshopItemCallResult.Set(hSteamAPICall, this, &SteamWorkshop::workshopItemCreated);
}

void SteamWorkshop::workshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure)
        return;

    m_UGCUpdateHandle = SteamUGC()->StartItemUpdate(m_appID, pCallback->m_nPublishedFileId);
    emit workshopItemCreated(pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement, pCallback->m_eResult, pCallback->m_nPublishedFileId);
}

void SteamWorkshop::submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, const QUrl projectFile, const QUrl videoFile, int publishedFileId)
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
    projectConfig.setFileName(absoluteContentPath + "/project.json");

    QJsonObject jsonObject;
    QJsonDocument jsonProject;
    QJsonParseError parseError;

    projectConfig.open(QIODevice::ReadOnly);
    QString projectConfigData = projectConfig.readAll();
    jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        emit remoteWorkshopCreationStatusChanged(RemoteWorkshopCreationStatus::ErrorUnknown);
        return;
    }

    jsonObject = jsonProject.object();
    QString preview = absoluteContentPath + "/" + jsonObject.value("preview").toString();

    // Only now we have an workshop id, so we have to add this for later use
    if (!jsonObject.contains("workshopid")) {
        jsonObject.insert("workshopid", publishedFileId);
    }
    projectConfig.close();
    // Reopen to empty the file via Truncate
    projectConfig.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream out(&projectConfig);
    out << QJsonDocument(jsonObject).toJson();
    projectConfig.close();

    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toLatin1()).data());
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toLatin1()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle, QByteArray(language.toLatin1()).data());
    SteamUGC()->SetItemContent(m_UGCUpdateHandle, QByteArray(absoluteContentPath.toLatin1()).data());
    SteamUGC()->SetItemPreview(m_UGCUpdateHandle, QByteArray(preview.toLatin1()).data());
    auto tagList = new QScopedPointer<SteamParamStringArray_t>(new SteamParamStringArray_t);

    //const char** cchKey[5][2000];

    //tagList->data()->m_ppStrings = cchKey;
    //tagList->data()->m_nNumStrings = 1;
    //tagList->data()->m_ppStrings[1][0]  = QByteArray("asas").data();
    SteamUGC()->SetItemTags(m_UGCUpdateHandle, tagList->data());

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


void SteamWorkshop::subscribeItem(unsigned int id)
{
    SteamUGC()->SubscribeItem(static_cast<unsigned long long>(id));
}

void SteamWorkshop::searchWorkshop()
{
    m_UGCSearchHandle = SteamUGC()->CreateQueryAllUGCRequest(EUGCQuery::k_EUGCQuery_RankedByVote,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_appID,
        m_appID,
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

    //QtConcurrent::run([this, pCallback]() {   });

    SteamUGCDetails_t details;
    const int urlLength = 200;
    char url[urlLength];
    uint32 previews = 0;
    uint32 subscriber = 0;

    // Tags
    uint32 keyValueTags = 0;
    const int cchKeySize = 2000;
    char* cchKey[cchKeySize];
    const int cchValueSize = 2000;
    char* pchValue[cchValueSize];

    uint32 results = pCallback->m_unTotalMatchingResults;

    for (uint32 i = 0; i < results; i++) {
        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
            //qDebug() << "ok " << pCallback;
            if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url, static_cast<uint32>(urlLength))) {
                QByteArray urlData(url);

                //Todo use multiple preview for gif hover effect
                previews = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);
                if (i == 0) {
                    m_workshopListModel->setBannerWorkshopItem(details.m_nPublishedFileId, QString(details.m_rgchTitle), QUrl(urlData), 0);
                    emit workshopSearched();
                } else {
                    emit workshopSearchResult(details.m_nPublishedFileId, QString(details.m_rgchTitle), QUrl(urlData), 0);
                }
                //                    keyValueTags = SteamUGC()->GetQueryUGCNumKeyValueTags(pCallback->m_handle, i);
                //                    if (keyValueTags >= 0) {
                //                        if (SteamUGC()->GetQueryUGCKeyValueTag(pCallback->m_handle, i, 0, cchKey, cchKeySize, cchValueSize, pchValue)) {
                //                            qDebug() << QByteArray(pchKey) << QByteArray(pchValue);
                //                        }
                //                    }
                //                    if(SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle,i,EItemStatistic::k_EItemStatistic_NumSubscriptions,&subscriber)){

                //                    }
            }
        } else {
            qDebug() << "Loading error!";
        }
    }
    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
}

void SteamWorkshop::onWorkshopItemInstalled(ItemInstalled_t* itemInstalled)
{
    emit workshopItemInstalled(itemInstalled->m_unAppID, itemInstalled->m_nPublishedFileId);
}
