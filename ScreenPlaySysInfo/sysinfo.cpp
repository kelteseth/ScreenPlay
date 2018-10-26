#include "sysinfo.h"

SysInfo::SysInfo(QQuickItem *parent):
    QQuickItem(parent)
{
    // FIXME Elias 2018 QQmlEngine cannot handle smartpointers yet....
    m_ram = new RAM();
    m_cpu = new CPU();
}

SysInfo::~SysInfo()
{
    delete m_cpu;
    delete m_ram;
}
