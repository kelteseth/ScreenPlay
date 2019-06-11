#pragma once

#include "profile.h"
#include <QAbstractListModel>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>

/*!
    \class Profile List Model
    \brief Use to save all active wallpapers and widgets position and configurations after a restart
    \todo  Implement this!
           - Add a profile for a combination of active wallpaper and widgets!

*/
namespace ScreenPlay {
struct Profile;

class ProfileListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProfileListModel(QObject* parent = nullptr);

    enum RoleNames {
        NameRole = Qt::UserRole,
        NumberRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    void loadProfiles();
    bool getProfileByName(QString id, Profile* profile);
    QUrl m_localStoragePath;

private:
    QHash<int, QByteArray> m_roleNames;
    QVector<Profile> m_profileList;
};
}
