#pragma once

#include "cpu.h"
#include "ram.h"
#include <QQuickItem>
#include <QSharedPointer>
#include <memory>

class SysInfo : public QQuickItem {
    Q_OBJECT
    Q_DISABLE_COPY(SysInfo)

    Q_PROPERTY(RAM* ram READ ram NOTIFY ramChanged)
    Q_PROPERTY(CPU* cpu READ cpu NOTIFY cpuChanged)

public:
    SysInfo(QQuickItem* parent = nullptr);
    ~SysInfo();

    RAM* ram() const
    {
        return m_ram;
    }

    CPU* cpu() const
    {
        return m_cpu;
    }

public slots:

signals:
    void ramChanged(RAM* ram);
    void cpuChanged(CPU* cpu);

private:
    RAM* m_ram;
    CPU* m_cpu;
};
