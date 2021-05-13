#include "gpu.h"

#include "infoware/gpu.hpp"
#include "infoware/version.hpp"
#include <QDebug>

static const char* vendor_name(iware::gpu::vendor_t vendor) noexcept
{
    switch (vendor) {
    case iware::gpu::vendor_t::intel:
        return "Intel";
    case iware::gpu::vendor_t::amd:
        return "AMD";
    case iware::gpu::vendor_t::nvidia:
        return "NVidia";
    case iware::gpu::vendor_t::microsoft:
        return "Microsoft";
    case iware::gpu::vendor_t::qualcomm:
        return "Qualcomm";
    case iware::gpu::vendor_t::apple:
        return "Apple";
    default:
        return "Unknown";
    }
}

/*!
    \class GPU
    \inmodule ScreenPlaySysInfo
    \brief  .

*/
GPU::GPU(QObject* parent)
    : QObject(parent)
{
    const auto device_properties = iware::gpu::device_properties();
    if (device_properties.empty()) {
        qWarning() << "No detection methods enabled";
        return;
    }

    for (auto i = 0u; i < device_properties.size(); ++i) {
        const auto& properties_of_device = device_properties[i];

        // Skip Windows default
        if (QString::fromStdString(properties_of_device.name) == "Basic Render Driver")
            continue;

        setVendor(vendor_name(properties_of_device.vendor));
        setName(QString::fromStdString(properties_of_device.name));
        setRamSize(properties_of_device.memory_size);
        setCacheSize(properties_of_device.cache_size);
        setMaxFrequency(properties_of_device.max_frequency);
    }
}
