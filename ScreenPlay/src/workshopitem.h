#pragma once

#include <QString>
#include <QUrl>
#include <QtDebug>

/*!
    \class Steam Workshop Item
    \brief Used by the Steam Workshop List Model Class

*/


class WorkshopItem {
public:
    WorkshopItem();
    WorkshopItem(unsigned int id, QString title, QUrl previewImageUrl, int numSubscriptions);
    ~WorkshopItem();
    QUrl m_previewImageUrl;
    QString m_title;
    int m_numSubscriptions;
    unsigned int m_id;
};


