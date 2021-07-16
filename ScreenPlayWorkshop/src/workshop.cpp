#include "workshop.h"

namespace ScreenPlayWorkshop {
Workshop::Workshop(QQuickItem* parent)
    : QQuickItem(parent)
{
    m_installedListModel = std::make_unique<InstalledListModel>();
    m_steamWorkshop = std::make_unique<SteamWorkshop>(672870);
    m_installedListModel->init();
    qInfo() << "Workshop init";
}

}
