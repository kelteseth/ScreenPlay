// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/projectsettingslistmodel.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace ScreenPlay {

/*!
    \class ScreenPlay::ProjectSettingsListModel
    \inmodule ScreenPlay
    \brief ProjectSettingsListModel used for the dynamic loading of the properties json object inside a project.json.

    The loaded properties are allowed to have one level of nesting to simulate headlines:
    \code
    "properties": {
        "Attractor": {
            "attStrength": {
                "from": 0,
                "stepSize": 100,
                "to": 100000,
                "type": "slider",
                "value": 8000000
            }
        },
        "Emitter": {
            "emitRate": {
                "from": 0,
                "stepSize": 1,
                "to": 2500,
                "type": "slider",
                "value": 25
            }
        }
    }
    \endcode
    In this example the Attractor and Emitter would be a headline. In the UI we then displays it for the user to modify.
    Otherwhise it is a regular QAbstractListModel based list model.
*/

/*!
  \brief Constructor when loading properties from profiles.json
  We need to _flatten_ the json to make it work with a flat list model!
  See \sa getActiveSettingsJson to make the flat list to a hierarchical json object
*/
void ProjectSettingsListModel::init(const ContentTypes::InstalledType& type, const QJsonObject& properties)
{
    for (QJsonObject::const_iterator itParent = properties.begin(); itParent != properties.end(); itParent++) {
        // The first object is always a category
        const QString categoryName = itParent.key();
        const QJsonObject category = properties.value(categoryName).toObject();

        // Add the category header
        append(SettingsItem { categoryName });

        // Children of this category
        for (QJsonObject::const_iterator itChild = category.begin(); itChild != category.end(); itChild++) {
            const QString childName = itChild.key();
            const QJsonObject childValue = category.value(childName).toObject();

            // Create a settings item with the child name, value, and category
            append(SettingsItem { childName, childValue, categoryName });
        }
    }
}

/*!
  \brief ProjectSettingsListModel::getActiveSettingsJson
*/
QJsonObject ProjectSettingsListModel::getActiveSettingsJson()
{
    if (m_projectSettings.isEmpty()) {
        qWarning() << "Trying to read empty projectSettings. Abort!";
        return {};
    }

    // This creates a hierarchical json from the flat list model:
    // Category 1               // <- this is a headline item
    //    emitRate:
    //      "from": 0,
    //      "stepSize": 1,
    //      "to": 2500,
    //      "type": "slider",
    //      "value": 25
    // Category 2
    //    property 1
    // ...
    QString currentCategory;
    QJsonObject ret;
    std::unique_ptr<QJsonObject> properties = std::make_unique<QJsonObject>();

    for (auto& item : m_projectSettings) {

        if (item.m_isHeadline) {

            // It is empty the first time
            if (!properties->isEmpty()) {

                // We only write the current category right _before_
                // we start with the next one. This way we need to save
                // the last category below!

                ret.insert(currentCategory, *properties);
            }

            currentCategory = item.m_name;
            properties = std::make_unique<QJsonObject>();

        } else {
            properties->insert(item.m_name, item.m_value);
        }

        // Because the last item is never a headline,
        // we need to add the last category here because
        if (&item == &m_projectSettings.last()) {
            ret.insert(currentCategory, *properties);
        }
    }
    return ret;
}

/*!
    \brief appends an SettingsItem.
*/
void ProjectSettingsListModel::append(const SettingsItem&& item)
{
    beginInsertRows(QModelIndex(), m_projectSettings.size(), m_projectSettings.size());
    m_projectSettings.append(item);
    endInsertRows();
}

/*!
    \brief .
*/
void ProjectSettingsListModel::setValueAtIndex(const int row, const QString& key, const QString& category, const QJsonObject& value)
{
    if (row >= m_projectSettings.size() || row < 0) {
        qWarning() << "Cannot setValueAtIndex when index is out of bounce! Row: " << row << ", m_projectSettings size: " << m_projectSettings.size();
        return;
    }

    if (m_projectSettings.at(row).m_category != category) {
        qWarning() << "Category of the element does not match current settings";
        return;
    }

    if (m_projectSettings.at(row).m_isHeadline) {
        qWarning() << "Cannot set settings item from type headline!";
        return;
    }

    if (m_projectSettings.at(row).m_name != key) {
        qWarning() << "Name of the element does not match current settings";
        return;
    }

    m_projectSettings.replace(row, SettingsItem { key, value, category });

    QVector<int> roles = { ValueRole };

    emit dataChanged(index(row, 0), index(row, 0), roles);
}

/*!
    \brief .
*/
int ProjectSettingsListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_projectSettings.count();
}

/*!
    \brief .
*/
QVariant ProjectSettingsListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int rowIndex = index.row();

    if (index.row() < rowCount())
        switch (role) {
        case NameRole:
            return m_projectSettings.at(rowIndex).m_name;
        case IsHeadlineRole:
            return m_projectSettings.at(rowIndex).m_isHeadline;
        case ValueRole: {
            auto value = m_projectSettings.at(rowIndex).m_value;
            qDebug() << "ValueRole data:" << value; // Add this debug line
            return QVariant::fromValue(value);
        }
        case CategoryRole:
            return m_projectSettings.at(rowIndex).m_category;
        default:
            qWarning() << "Could not determine row for ProjectSettingsListModel";
            return QVariant();
        }

    return QVariant();
}

/*!
    \brief .
*/
QHash<int, QByteArray> ProjectSettingsListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { IsHeadlineRole, "isHeadline" },
        { ValueRole, "value" },
        { CategoryRole, "category" },
    };
    return roles;
}

}

#include "moc_projectsettingslistmodel.cpp"
