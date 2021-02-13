#include "sysinfo.h"

SysInfo::SysInfo(QQuickItem* parent)
    : QQuickItem(parent)
    , m_ram(std::make_unique<RAM>())
    , m_cpu(std::make_unique<CPU>())
    , m_storage(std::make_unique<Storage>())
    , m_uptime(std::make_unique<Uptime>())
{
}
