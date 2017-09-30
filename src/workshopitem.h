#ifndef WORKSHOPITEM_H
#define WORKSHOPITEM_H

#include <QUrl>
#include <QString>

class WorkshopItem
{
public:
    WorkshopItem(QString title, QUrl previewImageUrl);
    QUrl m_previewImageUrl;
    QString m_title;
};

#endif // WORKSHOPITEM_H
