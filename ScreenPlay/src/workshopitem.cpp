#include "workshopitem.h"

WorkshopItem::WorkshopItem(){

}
WorkshopItem::WorkshopItem(unsigned int id, QString title, QUrl previewImageUrl, QString type, int numSubscriptions)
{
    m_id = id;
    m_title = title;
    m_previewImageUrl = previewImageUrl;
    m_numSubscriptions = numSubscriptions;
    m_type = type;
}

WorkshopItem::~WorkshopItem()
{

}
