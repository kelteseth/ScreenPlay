#ifndef WORKSHOPITEM_H
#define WORKSHOPITEM_H

#include <QString>
#include <QUrl>
#include <QtDebug>

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

#endif // WORKSHOPITEM_H
