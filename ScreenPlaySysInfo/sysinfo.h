#pragma once

#include <QQuickItem>
#include <memory>

#include "cpu.h"
#include "ram.h"
#include "storage.h"

class SysInfo : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(RAM* ram READ ram NOTIFY ramChanged)
    Q_PROPERTY(CPU* cpu READ cpu NOTIFY cpuChanged)
    Q_PROPERTY(Storage* storage READ storage NOTIFY storageChanged)

public:
    SysInfo(QQuickItem* parent = nullptr);
    ~SysInfo() { }

    RAM* ram() const
    {
        return m_ram.get();
    }

    CPU* cpu() const
    {
        return m_cpu.get();
    }

    Storage* storage() const
    {
        return m_storage.get();
    }


signals:
    void ramChanged(RAM* ram);
    void cpuChanged(CPU* cpu);
    void storageChanged(Storage* storage);

private:
    std::unique_ptr<RAM> m_ram;
    std::unique_ptr<CPU> m_cpu;
    std::unique_ptr<Storage> m_storage;
};
