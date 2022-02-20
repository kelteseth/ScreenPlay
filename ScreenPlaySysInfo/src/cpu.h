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

#include <QDebug>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QQmlEngine>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
// https://github.com/rainmeter/rainmeter/blob/master/Library/MeasureCPU.cpp

class CPU : public QObject {
    Q_OBJECT
    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)
    Q_PROPERTY(int tickRate READ tickRate WRITE setTickRate NOTIFY tickRateChanged)
    QML_ELEMENT

public:
    explicit CPU(QObject* parent = nullptr);

    float usage() const
    {
        return m_usage;
    }

    int tickRate() const
    {
        return m_tickRate;
    }

signals:

    void usageChanged(float usage);

    void tickRateChanged(int tickRate);

public slots:
    void update();

    void setUsage(float usage)
    {
        if (qFuzzyCompare(m_usage, usage))
            return;

        m_usage = usage;
        emit usageChanged(m_usage);
    }

    void setTickRate(int tickRate)
    {
        if (m_tickRate == tickRate)
            return;

        qDebug() << "hat sich was geändert";

        m_tickRate = tickRate;
        emit tickRateChanged(m_tickRate);
    }

private:
    float m_usage = 0.0f;

    uint64_t lastIdleTime = 0;
    uint64_t lastKernelTime = 0;
    uint64_t lastUserTime = 0;

    int m_tickRate = 1000;
    QTimer m_updateTimer;
};
