// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QQuickItem>
#include <QtDebug>
#include <QtQml>

#include "installedlistfilter.h"
#include "installedlistmodel.h"
#include "steamworkshop.h"

namespace ScreenPlayWorkshop {

class ScreenPlayWorkshop : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // Prefix :: to tell the compiler its a namespace
    Q_PROPERTY(::ScreenPlayWorkshop::InstalledListModel* installedListModel READ installedListModel NOTIFY installedListModelChanged)
    Q_PROPERTY(::ScreenPlayWorkshop::InstalledListFilter* installedListFilter READ installedListFilter NOTIFY installedListFilterChanged)
    Q_PROPERTY(::ScreenPlayWorkshop::SteamWorkshop* steamWorkshop READ steamWorkshop NOTIFY steamWorkshopChanged)
    Q_PROPERTY(QUrl contentPath READ contentPath WRITE setContentPath NOTIFY contentPathChanged)

public:
    explicit ScreenPlayWorkshop();
    ~ScreenPlayWorkshop() { qInfo() << "ScreenPlayWorkshop destructor"; }

    InstalledListModel* installedListModel() const { return m_installedListModel.get(); }
    InstalledListFilter* installedListFilter() const { return m_installedListFilter.get(); }
    SteamWorkshop* steamWorkshop() const { return m_steamWorkshop.get(); }
    QUrl contentPath() const { return m_contentPath; }

public slots:
    bool init()
    {
        // Use custom content path if set, otherwise use QSettings default
        if (!m_contentPath.isEmpty()) {
            m_installedListModel->init(m_contentPath);
        } else {
            m_installedListModel->init();
        }
        m_installedListFilter->setSourceModel(m_installedListModel.get());
        return steamWorkshop()->init();
    }

    void setContentPath(const QUrl& contentPath)
    {
        if (m_contentPath == contentPath)
            return;
        m_contentPath = contentPath;
        m_installedListModel->setAbsoluteStoragePath(contentPath);
        emit contentPathChanged(m_contentPath);
    }

    void setInstalledListModel(InstalledListModel* installedListModel)
    {
        if (m_installedListModel.get() == installedListModel)
            return;

        m_installedListModel.reset(installedListModel);
        emit installedListModelChanged(m_installedListModel.get());
    }

    void setSteamWorkshop(SteamWorkshop* steamWorkshop)
    {
        if (m_steamWorkshop.get() == steamWorkshop)
            return;

        m_steamWorkshop.reset(steamWorkshop);
        emit steamWorkshopChanged(m_steamWorkshop.get());
    }

signals:
    void installedListModelChanged(InstalledListModel* installedListModel);
    void installedListFilterChanged();
    void steamWorkshopChanged(SteamWorkshop* steamWorkshop);
    void contentPathChanged(const QUrl& contentPath);

private:
    std::unique_ptr<InstalledListModel> m_installedListModel;
    std::unique_ptr<InstalledListFilter> m_installedListFilter;
    std::unique_ptr<SteamWorkshop> m_steamWorkshop;
    QUrl m_contentPath;
};
}
