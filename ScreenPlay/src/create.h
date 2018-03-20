#pragma once

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QString>
#include <QFile>

#include "qmlutilities.h"
#include "settings.h"

class Create : public QObject
{
    Q_OBJECT
public:
    explicit Create(Settings* st, QMLUtilities* util,QObject *parent = nullptr);

signals:

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);

private:
    Settings* m_settings;
    QMLUtilities* m_utils;
};
