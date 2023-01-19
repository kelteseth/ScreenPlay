// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QTimer>

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
