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

namespace ScreenPlay {

struct Profile;

class ProfileListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProfileListModel(
        const std::shared_ptr<GlobalVariables>& globalVariables,
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
    const std::shared_ptr<GlobalVariables>& m_globalVariables;
};
}
