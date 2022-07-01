#pragma once

#include <QQuickItem>
#include <QtDebug>
#include <QtQml>

#include "installedlistmodel.h"
#include "steamworkshop.h"

namespace ScreenPlayWorkshop {

class ScreenPlayWorkshop : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(InstalledListModel* installedListModel READ installedListModel NOTIFY installedListModelChanged)
    Q_PROPERTY(SteamWorkshop* steamWorkshop READ steamWorkshop NOTIFY steamWorkshopChanged)

public:
    explicit ScreenPlayWorkshop();
    ~ScreenPlayWorkshop() { qInfo() << "ScreenPlayWorkshop destructor"; }

    InstalledListModel* installedListModel() const { return m_installedListModel.get(); }
    SteamWorkshop* steamWorkshop() const { return m_steamWorkshop.get(); }

public slots:
    bool init()
    {
        return steamWorkshop()->init();
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
    void workshopListModelLoaded(SteamWorkshopListModel* li);
    void installedListModelChanged(InstalledListModel* installedListModel);
    void steamWorkshopChanged(SteamWorkshop* steamWorkshop);

private:
    std::unique_ptr<InstalledListModel> m_installedListModel;
    std::unique_ptr<SteamWorkshop> m_steamWorkshop;
};
}
