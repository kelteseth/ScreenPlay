#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>

#include "projectsettingslistitem.h"

namespace ScreenPlay {

class ProjectSettingsListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProjectSettingsListModel(QString file, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum ProjectSettingsRole {
        NameRole = Qt::UserRole,
        IsHeadlineRole,
        ValueRole,
    };
    Q_ENUM(ProjectSettingsRole)

    void init(QString file);

public slots:
    void append(QString name, bool isHeadline, QVariant value);

private:
    QVector<ProjectSettingsListItem> m_projectSettings;
};
}
