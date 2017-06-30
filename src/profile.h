#ifndef PROFILE_H
#define PROFILE_H

#include <QRect>
#include <QString>
#include <QUrl>

class Profile {
public:
    Profile();
    Profile(QUrl absolutePath, QString id, QString version, QString wallpaperId, QRect rect, bool isLooping);

    QUrl m_absolutePath;
    QString m_id;
    QString m_version = "";
    QString m_wallpaperId = "";
    QRect m_rect;
    bool m_isLooping = true;
};

#endif // PROFILE_H
