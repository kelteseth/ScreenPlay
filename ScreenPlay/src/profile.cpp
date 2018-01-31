#include "profile.h"

Profile::Profile()
{

}

Profile::Profile(QUrl absolutePath, QString id, QString version, QString wallpaperId, QRect rect, bool isLooping)
{
    m_absolutePath = absolutePath;
    m_id = id;
    m_version = version;
    m_wallpaperId = wallpaperId;
    m_rect = rect;
    m_isLooping = isLooping;
}

