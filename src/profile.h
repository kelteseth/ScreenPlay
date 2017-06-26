#ifndef PROFILE_H
#define PROFILE_H

#include <QRect>
#include <QString>

class Profile {
public:
    Profile();
    Profile(QString id, QString version, QString wallpaperId, QRect rect, bool isLooping);

    QString m_id;
    QString m_version = "";
    QString m_wallpaperId = "";
    QRect m_rect;
    bool m_isLooping = true;
};

#endif // PROFILE_H
