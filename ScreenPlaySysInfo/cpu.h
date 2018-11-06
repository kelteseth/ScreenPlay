#pragma once

#include <QObject>
#include <QDebug>
#include <QTimer>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

// https://github.com/rainmeter/rainmeter/blob/master/Library/MeasureCPU.cpp

typedef LONG(WINAPI* FPNTQSI)(UINT, PVOID, ULONG, PULONG);

class CPU : public QObject {
    Q_OBJECT

    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)
    Q_PROPERTY(int tickRate READ tickRate WRITE setTickRate NOTIFY tickRateChanged)

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
        qWarning("Floating point comparison needs context sanity check");
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
    float m_usage = 42.0f;

    int m_Processor;

    double m_OldTime[2];

    static FPNTQSI c_NtQuerySystemInformation;

    static int c_NumOfProcessors;
    static ULONG c_BufferSize;
    int m_tickRate;
};
