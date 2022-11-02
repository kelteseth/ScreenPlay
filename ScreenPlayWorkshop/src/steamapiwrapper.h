#include <QMap>
#include <QStringList>
#include <QVariant>

#include "steam/steam_qt_enums_generated.h"
#include "steam/steamtypes.h"

namespace SteamApiWrapper {
struct ItemUpdateData {
    QString m_title;
    QString m_description;
    QString m_updateLanguage;
    QString m_metadata;
    ScreenPlayWorkshopSteamEnums::ERemoteStoragePublishedFileVisibility visibility;
    QStringList m_tags;
    QString m_content; // update item content from this local folder
    QString m_preview;
    QMap<QString, QString> m_keyValueTag; // add new key-value tags for the item. Note that there can be multiple values for a tag.
    QString m_changeNote;
    QPair<QString, ScreenPlayWorkshopSteamEnums::EItemPreviewType> m_previewFile;
    QPair<QString, ScreenPlayWorkshopSteamEnums::EItemPreviewType> m_previewVideo;

    const char* title() const { return m_title.toUtf8().data(); }
    const char* description() const { return m_description.toUtf8().data(); }
    const char* updateLanguage() const { return m_updateLanguage.toUtf8().data(); }
    const char* content() const { return m_content.toUtf8().data(); }
    const char* preview() const { return m_preview.toUtf8().data(); }
};

bool setItemTags(const QVariant& updateHandle, const QStringList tags);
// SteamAPICall_t submitItemUpdate(const int appID, const QVariant& updateHandle, ItemUpdateData data);
}
