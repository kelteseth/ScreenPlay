#include "ram.h"
#include <QtQml/qqml.h>
#include <qmetatype.h>
#include "sysinfoapi.h"

RAM::RAM(QObject* parent)
    : QObject(parent)
{
    connect(&m_updateTimer,&QTimer::timeout,this,&RAM::update);
    m_updateTimer.start(m_tickRate);
}

void RAM::update()
{
    //Get values from system
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX); //needed for internal api
    bool success = GlobalMemoryStatusEx(&memoryStatus);
    if(!success)
        return;

    //Apply total values
    setTotalPhysicalMemory(memoryStatus.ullTotalPhys);
    setTotalPagingMemory(memoryStatus.ullTotalPageFile);
    setTotalVirtualMemory(memoryStatus.ullTotalVirtual);

    //calculate usages
    setUsedPhysicalMemory(m_totalPhysicalMemory - memoryStatus.ullAvailPhys);
    setUsedPagingMemory(m_totalPagingMemory - memoryStatus.ullAvailPageFile);
    setUsedVirtualMemory(m_totalVirtualMemory - memoryStatus.ullAvailVirtual);

    //set overall usage
    setUsage(memoryStatus.dwMemoryLoad);
}
