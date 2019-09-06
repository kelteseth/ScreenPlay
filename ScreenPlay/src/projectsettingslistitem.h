#pragma once

#include <QString>
#include <QVariant>

/*!
    \class Project Settings List Item
    \brief List Item for Project Settings List Model. See Project Settings List Model for more informations


*/
namespace ScreenPlay {

struct ProjectSettingsListItem {

    ProjectSettingsListItem(
        const QString& name,
        const bool isHeadline,
        const QVariant& value)
    {
        m_name = name;
        m_isHeadline = isHeadline;
        m_value = value;
    }
    ProjectSettingsListItem() {}

    QString m_name;
    bool m_isHeadline { false };
    QVariant m_value;
};
}
