#pragma once

#include <QString>
#include <QVariant>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProjectSettingsListItem
    \inmodule ScreenPlay
    \brief List Item for Project Settings List Model. See ProjectSettingsListModel for more informations.


*/

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
