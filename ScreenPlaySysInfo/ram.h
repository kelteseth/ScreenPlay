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

#include <QObject>
#include <QDebug>
#include <QTimer>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
//#include <sysinfoapi.h>

#define DWORDLONG unsigned long long

class RAM : public QObject {
    Q_OBJECT

    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)

    Q_PROPERTY(unsigned long long usedPhysicalMemory READ usedPhysicalMemory NOTIFY usedPhysicalMemoryChanged)
    Q_PROPERTY(unsigned long long totalPhysicalMemory READ totalPhysicalMemory NOTIFY totalPhysicalMemoryChanged)

    Q_PROPERTY(unsigned long long usedVirtualMemory READ usedVirtualMemory NOTIFY usedVirtualMemoryChanged)
    Q_PROPERTY(unsigned long long totalVirtualMemory READ totalVirtualMemory NOTIFY totalVirtualMemoryChanged)

    Q_PROPERTY(unsigned long long usedPagingMemory READ usedPagingMemory NOTIFY usedPagingMemoryChanged)
    Q_PROPERTY(unsigned long long totalPagingMemory READ totalPagingMemory NOTIFY totalPagingMemoryChanged)

public:
    explicit RAM(QObject* parent = nullptr);

    //total memory usage in percent
    float usage() const
    {
        return m_usage;
    }

    //used physical memory in byte
    DWORDLONG usedPhysicalMemory() const
    {
        return m_usedPhysicalMemory;
    }

    //total physical memory in byte
    DWORDLONG totalPhysicalMemory() const
    {
        return m_totalPhysicalMemory;
    }

    //total virtual memory in byte
    DWORDLONG totalVirtualMemory() const
    {
        return m_totalVirtualMemory;
    }

    //used virtual memory in byte
    DWORDLONG usedVirtualMemory() const
    {
        return m_usedVirtualMemory;
    }

    //used paging memory in byte
    DWORDLONG usedPagingMemory() const
    {
        return m_usedPagingMemory;
    }

    //total paging memory in byte
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
        if(usedPhysicalMemory == m_usedPhysicalMemory)
            return;

        m_usedPhysicalMemory = usedPhysicalMemory;
        emit usedPhysicalMemoryChanged(m_usedPhysicalMemory);
    }

    void setTotalPhysicalMemory(DWORDLONG totalPhysicalMemory)
    {
        if(totalPhysicalMemory == m_totalPhysicalMemory)
            return;

        m_totalPhysicalMemory = totalPhysicalMemory;
        emit totalPhysicalMemoryChanged(m_totalPhysicalMemory);
    }

    void setUsedVirtualMemory(DWORDLONG usedVirtualMemory)
    {
        if(usedVirtualMemory == m_usedVirtualMemory)
            return;

        m_usedVirtualMemory = usedVirtualMemory;
        emit usedVirtualMemoryChanged(m_usedVirtualMemory);
    }

    void setTotalVirtualMemory(DWORDLONG totalVirtualMemory)
    {
        if(totalVirtualMemory == m_totalVirtualMemory)
            return;

        m_totalVirtualMemory = totalVirtualMemory;
        emit totalVirtualMemoryChanged(m_totalVirtualMemory);
    }

    void setUsedPagingMemory(DWORDLONG usedPagingMemory)
    {
        if(usedPagingMemory == m_usedPagingMemory)
            return;

        m_usedPagingMemory = usedPagingMemory;
        emit usedPagingMemoryChanged(m_usedPagingMemory);
    }

    void setTotalPagingMemory(DWORDLONG totalPagingMemory)
    {
        if(totalPagingMemory == m_totalPagingMemory)
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

