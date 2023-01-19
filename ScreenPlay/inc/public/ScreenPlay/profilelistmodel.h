// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/profile.h"

#include <memory>

namespace ScreenPlay {

struct Profile;

class ProfileListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

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
