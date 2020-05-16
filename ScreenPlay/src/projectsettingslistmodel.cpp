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
  Constructor
*/
ProjectSettingsListModel::ProjectSettingsListModel(QString file, QObject* parent)
    : QAbstractListModel(parent)
{
    init(file);
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

    if (index.row() < rowCount())
        switch (role) {
        case NameRole:
            return m_projectSettings.at(index.row()).m_name;
        case IsHeadlineRole:
            return m_projectSettings.at(index.row()).m_isHeadline;
        case ValueRole:
            return m_projectSettings.at(index.row()).m_value;
        default:
            return QVariant();
        }

    return QVariant();
}

QHash<int, QByteArray> ProjectSettingsListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { NameRole, "name" },
        { IsHeadlineRole, "isHeadline" },
        { ValueRole, "value" },
    };
    return roles;
}

/*!
 Recursively loads the content of a project.json.
 See also \l {https://kelteseth.gitlab.io/ScreenPlayDocs/project/project/} .
 */
void ProjectSettingsListModel::init(QString file)
{
    QFile configTmp;
    configTmp.setFileName(file);
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;
    QJsonObject obj;

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();

    if(config.isEmpty()){
        qWarning() << "File: " << configTmp << " is empty!";
        return;
    }

    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qInfo() << "File: " << file;
        qInfo() << "Value: " << config;
        qInfo() <<parseError.error << parseError.errorString();
        qWarning("Settings Json Parse Error ");
        return;
    }

    obj = configJsonDocument.object();
    QJsonObject tmpParent;

    if (obj.contains("properties")) {
        tmpParent = obj.value("properties").toObject();
    } else {
        return;
    }

    QJsonObject::iterator itParent, itChild;
    for (itParent = tmpParent.begin(); itParent != tmpParent.end(); itParent++) {
        QJsonObject tmpChildObj = tmpParent.value(itParent.key()).toObject();
        append(itParent.key(), true, "");

        for (itChild = tmpChildObj.begin(); itChild != tmpChildObj.end(); itChild++) {
            append(itChild.key(), false, QJsonDocument(tmpChildObj.value(itChild.key()).toObject()).toJson());
        }
    }
}

void ProjectSettingsListModel::append(QString name, bool isHeadline, QVariant value)
{
    beginInsertRows(QModelIndex(), m_projectSettings.size(), m_projectSettings.size());

    ProjectSettingsListItem tmpFile(name, isHeadline, value);
    m_projectSettings.append(tmpFile);

    endInsertRows();
}
}
