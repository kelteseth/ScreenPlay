#pragma once
#include <QAbstractListModel>
#include <QFile>
#include <QQmlContext>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>

#include "projectsettingslistitem.h"

class ProjectSettingsListModel : public QAbstractListModel
{
    Q_OBJECT


public:
    explicit ProjectSettingsListModel(QString file, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum ProjectSettingsRole {
        NameRole,
        IsHeadlineRole,
        ValueRole,
    };
    Q_ENUM(ProjectSettingsRole)


public slots:
    void append(QString name, bool isHeadline, QVariant value);

private:
    QVector<ProjectSettingsListItem> m_projectSettings;
};
