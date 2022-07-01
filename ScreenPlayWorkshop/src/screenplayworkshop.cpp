#include "screenplayworkshop.h"

namespace ScreenPlayWorkshop {
ScreenPlayWorkshop::ScreenPlayWorkshop()
    : QObject(nullptr)
{
    qInfo() << "ScreenPlayWorkshop";
    m_installedListModel = std::make_unique<InstalledListModel>();
    m_steamWorkshop = std::make_unique<SteamWorkshop>();
    m_installedListModel->init();
}

}
