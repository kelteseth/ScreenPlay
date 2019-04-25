#pragma once
#include <QRect>
#include <QString>
#include <QUrl>

/*!
    \class Profile
    \brief  A classed used in combination with Profile List Model.
            See Profile List Model for more details

*/

struct Profile {

    Profile();
    Profile(QUrl absolutePath, QString id, QString version, QString wallpaperId, QRect rect, bool isLooping);

    QUrl m_absolutePath;
    QString m_id;
    QString m_version = "";
    QString m_wallpaperId = "";
    QRect m_rect;
    bool m_isLooping = true;
};
