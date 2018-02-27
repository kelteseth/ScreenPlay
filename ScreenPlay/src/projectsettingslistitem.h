#pragma once


#include <QString>
#include <QVariant>


class ProjectSettingsListItem
{

public:
    explicit ProjectSettingsListItem(QString name, bool isHeadline, QVariant value);
    ProjectSettingsListItem();
    ~ProjectSettingsListItem();
    QString m_name;
    bool m_isHeadline;
    QVariant m_value;
};
