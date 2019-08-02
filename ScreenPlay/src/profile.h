#pragma once
#include <QRect>
#include <QString>
#include <QUrl>

/*!
    \class Profile
    \brief  A classed used in combination with Profile List Model.
            See Profile List Model for more details

*/
namespace ScreenPlay {
struct Profile {

    Profile() {}
    Profile(const QUrl& absolutePath,
        const QString& id,
        const QString& version,
        const QString& wallpaperId,
        const QRect& rect,
        const bool isLooping)
    {
        m_absolutePath = absolutePath;
        m_id = id;
        m_version = version;
        m_wallpaperId = wallpaperId;
        m_rect = rect;
        m_isLooping = isLooping;
    }

    QUrl m_absolutePath;
    QString m_id;
    QString m_version = "";
    QString m_wallpaperId = "";
    QRect m_rect;
    bool m_isLooping = true;
    QVector<QString> m_monitorList;
};
}
