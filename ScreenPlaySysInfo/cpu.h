#pragma once

#include <QObject>
#include <QDebug>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

// https://github.com/rainmeter/rainmeter/blob/master/Library/MeasureCPU.cpp

typedef LONG(WINAPI* FPNTQSI)(UINT, PVOID, ULONG, PULONG);

class CPU : public QObject {
    Q_OBJECT

    Q_PROPERTY(float usage READ usage NOTIFY usageChanged)

public:
    explicit CPU(QObject* parent = nullptr);


    float usage() const
    {
        return m_usage;
    }

signals:

    void usageChanged(float usage);

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

private:
    float m_usage = 42.0f;

    int m_Processor;

    double m_OldTime[2];

    static FPNTQSI c_NtQuerySystemInformation;

    static int c_NumOfProcessors;
    static ULONG c_BufferSize;
};
