#include "projectsettingslistmodel.h"

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
  Constructor when loading properties from settings.json
*/

void ProjectSettingsListModel::init(const InstalledType::InstalledType& type, const QJsonObject& properties)
{
    if (type == InstalledType::InstalledType::VideoWallpaper) {
//        beginInsertRows(QModelIndex(), m_projectSettings.size(), m_projectSettings.size());

//        m_projectSettings.append("General");

//        if (properties.contains("volume"))
//            append({"slider", properties.value("volume")}=;

//        if (properties.contains("playbackRate"))
//            append("slider", properties.value("playbackRate").toObject());
//        endInsertRows();

    } else {
        for (QJsonObject::const_iterator itParent = properties.begin(); itParent != properties.end(); itParent++) {

            // The first object is always a category
            const QJsonObject category = properties.value(itParent.key()).toObject();
            beginInsertRows(QModelIndex(), m_projectSettings.size(), m_projectSettings.size());
            m_projectSettings.append(ProjectSettingsListItem { itParent.key() });
            endInsertRows();
            // Children of this category
            for (QJsonObject::const_iterator itChild = category.begin(); itChild != category.end(); itChild++) {
                qInfo() << itChild.key() << itChild.value();
                beginInsertRows(QModelIndex(), m_projectSettings.size(), m_projectSettings.size());
                m_projectSettings.append(ProjectSettingsListItem {itChild.key(), itChild.value().toObject() });
                endInsertRows();
            }
        }
    }
}

int ProjectSettingsListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_projectSettings.count();
}

QVariant ProjectSettingsListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int rowIndex = index.row();

    if (index.row() < rowCount())
        switch (role) {
        case NameRole:
            return m_projectSettings.at(rowIndex).m_key;
        case IsHeadlineRole:
            return m_projectSettings.at(rowIndex).m_isHeadline;
        case ValueRole:
            return m_projectSettings.at(rowIndex).m_value;
        default:
            qWarning() << "Could not determine row for ProjectSettingsListModel";
            return QVariant();
        }

    return QVariant();
}

QHash<int, QByteArray> ProjectSettingsListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "m_name" },
        { IsHeadlineRole, "m_isHeadline" },
        { ValueRole, "m_value" },
    };
    return roles;
}

QJsonObject ProjectSettingsListModel::getActiveSettingsJson()
{
    if (m_projectSettings.isEmpty()) {
        qWarning() << "Trying to read emppty projectSettings. Abort!";
        return {};
    }

    QJsonObject obj;
    for (const auto& itemCategory : m_projectSettings) {
        QJsonObject category;
        if (itemCategory.m_isHeadline) {
            for (const auto& itemProperties : m_projectSettings) {
                if (itemProperties.m_isHeadline)
                    continue;
                category.insert(itemProperties.m_key, QJsonValue::fromVariant(itemProperties.m_value));
            }
            obj.insert(itemCategory.m_key, category);
        }
    }
    return obj;
}

}
