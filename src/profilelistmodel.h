#ifndef PROFILELISTMODEL_H
#define PROFILELISTMODEL_H

#include "profile.h"
#include <QAbstractListModel>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QDir>
#include <QVector>

class Profile;

class ProfileListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProfileListModel(QObject* parent = 0);

    enum RoleNames {
        NameRole = Qt::UserRole,
        NumberRole = Qt::UserRole + 2
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    void loadProfiles();

private:
    QHash<int, QByteArray> m_roleNames;
    QVector<Profile*> m_profileList;
};

#endif // PROFILELISTMODEL_H
