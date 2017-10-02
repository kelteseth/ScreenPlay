#ifndef WORKSHOPITEM_H
#define WORKSHOPITEM_H

#include <QString>
#include <QUrl>

class WorkshopItem {
public:
    WorkshopItem(QString title, QUrl previewImageUrl);
    ~WorkshopItem();
    QUrl m_previewImageUrl;
    QString m_title;
};

#endif // WORKSHOPITEM_H
