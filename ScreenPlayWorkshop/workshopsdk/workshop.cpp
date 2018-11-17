#include "workshop.h"

Workshop::Workshop(QQuickItem* parent) : QQuickItem(parent)
{
    m_aimberAPI.openConnection();
}

Workshop::~Workshop()
{
}
