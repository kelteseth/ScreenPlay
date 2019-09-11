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

#include "globalvariables.h"

#include <memory>

/*!
    \class Profile List Model
    \brief Use to save all active wallpapers and widgets position and configurations after a restart
    \todo  Implement this!
           - Add a profile for a combination of active wallpaper and widgets!

*/
namespace ScreenPlay {

using std::shared_ptr;

struct Profile;

class ProfileListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProfileListModel(
        const shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    enum RoleNames {
        NameRole = Qt::UserRole,
        NumberRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    bool getProfileByName(QString id, Profile* profile);
    void append(const Profile& profile);

private:
    QVector<Profile> m_profileList;
    const shared_ptr<GlobalVariables>& m_globalVariables;
};
}
