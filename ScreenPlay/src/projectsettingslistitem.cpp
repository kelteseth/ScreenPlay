#include "projectsettingslistitem.h"

ProjectSettingsListItem::ProjectSettingsListItem(QString name, bool isHeadline,  QVariant value)
{
    m_name = name;
    m_isHeadline = isHeadline;
    m_value = value;
}

ProjectSettingsListItem::ProjectSettingsListItem()
{

}

ProjectSettingsListItem::~ProjectSettingsListItem()
{

}

