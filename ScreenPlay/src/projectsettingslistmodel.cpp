#include "projectsettingslistmodel.h"

ProjectSettingsListModel::ProjectSettingsListModel(QString file, QObject* parent)
    : QAbstractListModel(parent)
{
    init(file);
}

ProjectSettingsListModel::ProjectSettingsListModel()
{
}

int ProjectSettingsListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_projectSettings.count();

    // FIXME: Implement me!
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
    static const QHash<int, QByteArray> roles{
        { NameRole, "name" },
        { IsHeadlineRole, "isHeadline" },
        { ValueRole, "value" },
    };
    return roles;
}

void ProjectSettingsListModel::init(QString file)
{
    qDebug() << file;
    QFile configTmp;
    configTmp.setFileName(file);
    QJsonDocument configJsonDocument;
    QJsonParseError parseError;
    QJsonObject obj;

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }

    obj = configJsonDocument.object();
    QJsonObject tmpParent;

    if (obj.contains("properties")) {
        tmpParent = obj.value("properties").toObject();
    } else {
        qWarning("Could not find settings");
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
