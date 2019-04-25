#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlContext>
#include <QVector>

#include "projectsettingslistitem.h"

/*!
    \class Project Settings List Model
    \brief Reads the project.json for every Wallpaper, Scene or Widget "general" object and displays it for the user to modify

*/

class ProjectSettingsListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProjectSettingsListModel(QString file, QObject* parent = nullptr);
    ProjectSettingsListModel();
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum ProjectSettingsRole {
        NameRole,
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
