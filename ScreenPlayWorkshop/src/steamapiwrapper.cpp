#include <QByteArray>
#include <QJsonArray>
#include <QString>
#include <QStringList>

#include "cstring"
#include "stdlib.h"
#include "steam/steam_api.h"
#include "steam/steamtypes.h"
#include "steamapiwrapper.h"

namespace SteamApiWrapper {
bool setItemTags(const QVariant& updateHandle, const QStringList tags)
{
    SteamParamStringArray_t* pTags = new SteamParamStringArray_t();
    const uint32 strCount = tags.size();
    pTags->m_ppStrings = new const char*[strCount];
    pTags->m_nNumStrings = strCount;
    for (uint32 i = 0; i < strCount; i++) {
        pTags->m_ppStrings[i] = tags.at(i).toUtf8().data();
    }

    return SteamUGC()->SetItemTags(updateHandle.toULongLong(), pTags);
}

}
