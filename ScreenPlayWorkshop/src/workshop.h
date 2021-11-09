#pragma once

#include <QQuickItem>
#include <QtDebug>
#include <QtQml>

#include "steam/steam_api.h"
#include "steam/steam_qt_enums_generated.h"

#include "installedlistmodel.h"
#include "steamworkshop.h"

namespace ScreenPlayWorkshop {

class Workshop : public QQuickItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(ScreenPlayWorkshop)
    Q_PROPERTY(InstalledListModel* installedListModel READ installedListModel NOTIFY installedListModelChanged)
    Q_PROPERTY(SteamWorkshop* steamWorkshop READ steamWorkshop NOTIFY steamWorkshopChanged)

public:
    Workshop(QQuickItem* parent = nullptr);
    ~Workshop() { qInfo() << "workshop destructor"; }

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
