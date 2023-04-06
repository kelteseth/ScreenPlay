// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#define DWORDLONG quint64

class RAM : public QObject {
    Q_OBJECT

    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)
    Q_PROPERTY(quint64 usedPhysicalMemory READ usedPhysicalMemory NOTIFY usedPhysicalMemoryChanged)
    Q_PROPERTY(quint64 totalPhysicalMemory READ totalPhysicalMemory NOTIFY totalPhysicalMemoryChanged)
    Q_PROPERTY(quint64 usedVirtualMemory READ usedVirtualMemory NOTIFY usedVirtualMemoryChanged)
    Q_PROPERTY(quint64 totalVirtualMemory READ totalVirtualMemory NOTIFY totalVirtualMemoryChanged)
    Q_PROPERTY(quint64 usedPagingMemory READ usedPagingMemory NOTIFY usedPagingMemoryChanged)
    Q_PROPERTY(quint64 totalPagingMemory READ totalPagingMemory NOTIFY totalPagingMemoryChanged)
    QML_ELEMENT

public:
    explicit RAM(QObject* parent = nullptr);

    // total memory usage in percent
    float usage() const
    {
        return m_usage;
    }

    // used physical memory in byte
    DWORDLONG usedPhysicalMemory() const
    {
        return m_usedPhysicalMemory;
    }

    // total physical memory in byte
    DWORDLONG totalPhysicalMemory() const
    {
        return m_totalPhysicalMemory;
    }

    // total virtual memory in byte
    DWORDLONG totalVirtualMemory() const
    {
        return m_totalVirtualMemory;
    }

    // used virtual memory in byte
    DWORDLONG usedVirtualMemory() const
    {
        return m_usedVirtualMemory;
    }

    // used paging memory in byte
    DWORDLONG usedPagingMemory() const
    {
        return m_usedPagingMemory;
    }

    // total paging memory in byte
    DWORDLONG totalPagingMemory() const
    {
        return m_totalPagingMemory;
    }

signals:

    void usageChanged(float usage);

    void usedPhysicalMemoryChanged(DWORDLONG usedPhysicalMemory);

    void usedVirtualMemoryChanged(DWORDLONG usedVirtualMemory);

    void usedPagingMemoryChanged(DWORDLONG usedPagingMemory);

    void totalPhysicalMemoryChanged(DWORDLONG totalPhysicalMemory);

    void totalVirtualMemoryChanged(DWORDLONG totalVirtualMemory);

    void totalPagingMemoryChanged(DWORDLONG totalPagingMemory);

private slots:
    void update();

    void setUsage(float usage)
    {
        if (qFuzzyCompare(m_usage, usage))
            return;

        m_usage = usage;
        emit usageChanged(m_usage);
    }

    void setUsedPhysicalMemory(DWORDLONG usedPhysicalMemory)
    {
        if (usedPhysicalMemory == m_usedPhysicalMemory)
            return;

        m_usedPhysicalMemory = usedPhysicalMemory;
        emit usedPhysicalMemoryChanged(m_usedPhysicalMemory);
    }

    void setTotalPhysicalMemory(DWORDLONG totalPhysicalMemory)
    {
        if (totalPhysicalMemory == m_totalPhysicalMemory)
            return;

        m_totalPhysicalMemory = totalPhysicalMemory;
        emit totalPhysicalMemoryChanged(m_totalPhysicalMemory);
    }

    void setUsedVirtualMemory(DWORDLONG usedVirtualMemory)
    {
        if (usedVirtualMemory == m_usedVirtualMemory)
            return;

        m_usedVirtualMemory = usedVirtualMemory;
        emit usedVirtualMemoryChanged(m_usedVirtualMemory);
    }

    void setTotalVirtualMemory(DWORDLONG totalVirtualMemory)
    {
        if (totalVirtualMemory == m_totalVirtualMemory)
            return;

        m_totalVirtualMemory = totalVirtualMemory;
        emit totalVirtualMemoryChanged(m_totalVirtualMemory);
    }

    void setUsedPagingMemory(DWORDLONG usedPagingMemory)
    {
        if (usedPagingMemory == m_usedPagingMemory)
            return;

        m_usedPagingMemory = usedPagingMemory;
        emit usedPagingMemoryChanged(m_usedPagingMemory);
    }

    void setTotalPagingMemory(DWORDLONG totalPagingMemory)
    {
        if (totalPagingMemory == m_totalPagingMemory)
            return;

        m_totalPagingMemory = totalPagingMemory;
        emit totalPagingMemoryChanged(m_totalPagingMemory);
    }

private:
    float m_usage = -1.f;
    DWORDLONG m_usedPhysicalMemory = 0ul;
    DWORDLONG m_totalVirtualMemory = 0ul;
    DWORDLONG m_totalPhysicalMemory = 0ul;
    DWORDLONG m_usedVirtualMemory = 0ul;
    DWORDLONG m_usedPagingMemory = 0ul;
    DWORDLONG m_totalPagingMemory = 0ul;

    QTimer m_updateTimer;
    int m_tickRate = 1000;
};
