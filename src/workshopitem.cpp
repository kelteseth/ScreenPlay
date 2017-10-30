#include "workshopitem.h"


WorkshopItem::WorkshopItem(unsigned int id, QString title, QUrl previewImageUrl)
{
    m_id = id;
    m_title = title;
    m_previewImageUrl = previewImageUrl;
}

WorkshopItem::~WorkshopItem()
{

}
