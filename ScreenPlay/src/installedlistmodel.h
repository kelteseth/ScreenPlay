/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QtConcurrent/QtConcurrent>

#include "globalvariables.h"
#include "profilelistmodel.h"
#include "projectfile.h"
#include "util.h"

#include <memory>

namespace ScreenPlay {

class InstalledListModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit InstalledListModel(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum InstalledRole {
        TitleRole = Qt::UserRole,
        TypeRole,
        PreviewRole,
        PreviewGIFRole,
        FolderIdRole,
        FileIdRole,
        AbsoluteStoragePathRole,
        WorkshopIDRole,
    };
    Q_ENUM(InstalledRole)

    int count() const
    {
        return m_count;
    }

public slots:
    void loadInstalledContent();
    void append(const QJsonObject&, const QString&);
    void reset();
    void init();
    QVariantMap get(QString folderId);

    void setCount(int count)
    {
        if (m_count == count)
            return;

        m_count = count;
        emit countChanged(m_count);
    }

signals:
    void setScreenVisible(bool visible);
    void setScreenToVideo(QString absolutePath);
    void addInstalledItem(const QJsonObject, const QString);
    void installedLoadingFinished();
    void isLoadingContentChanged(bool isLoadingContent);
    void countChanged(int count);

private:
    QFileSystemWatcher m_fileSystemWatcher;
    QVector<ProjectFile> m_screenPlayFiles;
    int m_count { 0 };

    const std::shared_ptr<GlobalVariables>& m_globalVariables;
};
}
