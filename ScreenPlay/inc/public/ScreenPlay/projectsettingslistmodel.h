// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include "ScreenPlayCore/contenttypes.h"
#include <QAbstractListModel>
#include <QJsonObject>
#include <QVector>

namespace ScreenPlay {

struct SettingsItem {
    SettingsItem(
        const QString& name,
        const QJsonObject& value,
        const QString& category)
    {
        m_name = name;
        m_isHeadline = false;
        m_value = value;
        m_category = category;
    }

    SettingsItem(
        const QString& name)
    {
        m_name = name;
        m_category = name;
        m_isHeadline = true;
    }
    QString m_name;
    QString m_category;
    bool m_isHeadline;
    QJsonObject m_value;

public:
    void setValue(const QJsonObject& value)
    {
        m_value = value;
    }
};

class ProjectSettingsListModel : public QAbstractListModel {
    Q_OBJECT

public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum ProjectSettingsRole {
        NameRole = Qt::UserRole,
        IsHeadlineRole,
        ValueRole,
        CategoryRole,
    };
    Q_ENUM(ProjectSettingsRole)

    QJsonObject getActiveSettingsJson();
    void init(const ContentTypes::InstalledType& type, const QJsonObject& properties);
    void append(const SettingsItem&& item);

    Q_INVOKABLE void setValueAtIndex(const int row, const QString& key, const QString& category, const QJsonObject& value);

private:
    QVector<SettingsItem> m_projectSettings;
};
}
