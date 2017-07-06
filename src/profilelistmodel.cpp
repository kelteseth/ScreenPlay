#include "profilelistmodel.h"

#include <QDirIterator>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPair>

ProfileListModel::ProfileListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

int ProfileListModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_profileList.count();
}

QVariant ProfileListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_profileList.count()) {
        return QVariant();
    }

    switch (role) {
    case NameRole:
        return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> ProfileListModel::roleNames() const
{
    return m_roleNames;
}

void ProfileListModel::loadProfiles()
{

    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/Profiles";
    if (!QDir(writablePath).exists()) {
        if (!QDir().mkdir(writablePath)) {
            qWarning("Error could not create Profiles folder");
            return;
        }
    }

    QFileInfoList list = QDir(writablePath).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
    QString tmpPath;
    QJsonDocument profileDoc;
    QJsonParseError parseError;
    QJsonObject profileObj;

    for (auto&& item : list) {
        tmpPath = writablePath + "/" + item.baseName() + "/profile.json";

        QFile settings;
        settings.setFileName(tmpPath);
        settings.open(QIODevice::ReadOnly | QIODevice::Text);
        QString projectConfigData = settings.readAll();
        profileDoc = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

        if (!(parseError.error == QJsonParseError::NoError))
            continue;

        profileObj = profileDoc.object();

        Profile tmpProfile;
        tmpProfile.m_id = item.baseName();

        if (profileObj.contains("version"))
            tmpProfile.m_version = profileObj.value("version").toString();

        if (profileObj.contains("wallpaperId"))
            tmpProfile.m_wallpaperId = profileObj.value("wallpaperId").toString();

        if (profileObj.contains("width") && profileObj.contains("height") && profileObj.contains("xPos") && profileObj.contains("yPos")) {
            //Check for inpossible values
            if (profileObj.value("width").toInt() == 0 || profileObj.value("height").toInt() == 0) {
                continue;
            }
            tmpProfile.m_rect.setWidth(profileObj.value("width").toInt());
            tmpProfile.m_rect.setHeight(profileObj.value("height").toInt());
            tmpProfile.m_rect.setX(profileObj.value("xPos").toInt());
            tmpProfile.m_rect.setY(profileObj.value("yPos").toInt());

        } else {
            qWarning("Parsing error");
        }

        if (profileObj.contains("isLooping"))
            tmpProfile.m_isLooping = profileObj.value("isLooping").toBool();

        m_profileList.append(tmpProfile);
    }
}

bool ProfileListModel::getProfileByName(QString id, Profile* profile)
{
    for (int i = 0; i < m_profileList.size(); i++) {
        if (m_profileList.at(i).m_id == id) {
            *profile = m_profileList.at(i);
            return true;
        }
    }
    return false;
}
