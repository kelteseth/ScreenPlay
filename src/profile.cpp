#include "profile.h"

Profile::Profile()
{

}

Profile::Profile(QString id, QString version, QString wallpaperId, QRect rect, bool isLooping)
{
    m_id = id;
    m_version = version;
    m_wallpaperId = wallpaperId;
    m_rect = rect;
    m_isLooping = isLooping;
}

