// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QQmlEngine>
#include <QQuickItem>
#include <memory>

#include "cpu.h"
#include "gpu.h"
#include "ram.h"
#include "storage.h"
#include "uptime.h"

class SysInfo : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(GPU* gpu READ gpu NOTIFY gpuChanged)
    Q_PROPERTY(RAM* ram READ ram NOTIFY ramChanged)
    Q_PROPERTY(CPU* cpu READ cpu NOTIFY cpuChanged)
    Q_PROPERTY(Storage* storage READ storage NOTIFY storageChanged)
    Q_PROPERTY(Uptime* uptime READ uptime NOTIFY uptimeChanged)
    QML_ELEMENT

public:
    SysInfo(QQuickItem* parent = nullptr);

    RAM* ram() const { return m_ram.get(); }
    CPU* cpu() const { return m_cpu.get(); }
    Storage* storage() const { return m_storage.get(); }
    Uptime* uptime() const { return m_uptime.get(); }
    GPU* gpu() const { return m_gpu.get(); }

signals:
    void ramChanged(RAM* ram);
    void cpuChanged(CPU* cpu);
    void storageChanged(Storage* storage);
    void uptimeChanged(Uptime* uptime);
    void gpuChanged(GPU*);

private:
    std::unique_ptr<RAM> m_ram;
    std::unique_ptr<CPU> m_cpu;
    std::unique_ptr<Storage> m_storage;
    std::unique_ptr<Uptime> m_uptime;
    std::unique_ptr<GPU> m_gpu;
};
