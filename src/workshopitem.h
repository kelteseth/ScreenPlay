#ifndef WORKSHOPITEM_H
#define WORKSHOPITEM_H

#include <QString>
#include <QUrl>
#include <QtDebug>

class WorkshopItem {
public:
    WorkshopItem(unsigned int id, QString title, QUrl previewImageUrl);
    ~WorkshopItem();
    QUrl m_previewImageUrl;
    QString m_title;
    unsigned int m_id;
};

#endif // WORKSHOPITEM_H
