#include "windowsutils.h"
#include <highlevelmonitorconfigurationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#pragma comment(lib, "Dxva2.lib")
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <devguid.h>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <windows.h>
#pragma comment(lib, "Setupapi.lib")
#define UNUSED(x) (void)x;

namespace ScreenPlay {

// Helper to convert monitor ID to manufacturer
std::string GetManufacturerName(const std::string& monitorID)
{
    static const std::map<std::string, std::string> manufacturers = {
        { "GSM", "LG" },
        { "HAT", "Hanstar" },
        { "SAM", "Samsung" },
        { "SEC", "Samsung" },
        { "HWP", "HP" },
        { "ACI", "Asus" },
        { "AUS", "Asus" },
        { "DEL", "Dell" },
        { "BNQ", "BenQ" },
        { "AOC", "AOC" },
        { "PHL", "Philips" },
        { "VSC", "ViewSonic" }
    };

    // Extract first 3 letters of monitor ID
    if (monitorID.length() >= 3) {
        std::string code = monitorID.substr(0, 3);
        auto it = manufacturers.find(code);
        if (it != manufacturers.end()) {
            return it->second;
        }
    }
    return "";
}

std::string GetMonitorModelFromDeviceID(const std::wstring& deviceID)
{
    std::string deviceIDStr(deviceID.begin(), deviceID.end());
    std::cout << "Parsing device ID: " << deviceIDStr << std::endl;

    // Extract monitor ID from DISPLAY#XXXXX# pattern
    std::regex displayPattern(R"(DISPLAY#([A-Za-z0-9]+)#)");
    std::smatch matches;
    if (std::regex_search(deviceIDStr, matches, displayPattern) && matches.size() > 1) {
        std::string monitorID = matches[1];
        std::cout << "Found monitor ID: " << monitorID << std::endl;

        // Get manufacturer name
        std::string manufacturer = GetManufacturerName(monitorID);

        // If we found a manufacturer, combine it with the model number
        if (!manufacturer.empty()) {
            std::string model = monitorID.substr(3); // Remove manufacturer code
            return manufacturer + " " + model;
        }

        // If no manufacturer found, just return the ID
        return monitorID;
    }
    return "";
}

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    UNUSED(hdcMonitor);
    UNUSED(lprcMonitor);
    std::vector<WindowsMonitor>* pMonitors = reinterpret_cast<std::vector<WindowsMonitor>*>(dwData);

    MONITORINFOEX info;
    info.cbSize = sizeof(info);
    GetMonitorInfo(hMonitor, &info);

    UINT dpiX, dpiY;
    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    WindowsMonitor monitor;
    monitor.monitorID = hMonitor;
    monitor.index = static_cast<int>(pMonitors->size());
    monitor.position = info.rcMonitor;
    monitor.size.cx = info.rcMonitor.right - info.rcMonitor.left;
    monitor.size.cy = info.rcMonitor.bottom - info.rcMonitor.top;
    monitor.scaleFactor = static_cast<float>(dpiX) / 96.0f;

    // Get device path
    DISPLAY_DEVICEW displayDevice = { sizeof(DISPLAY_DEVICEW), { 0 } };
    wchar_t deviceNameW[32];
    MultiByteToWideChar(CP_ACP, 0, info.szDevice, -1, deviceNameW, 32);

    std::string monitorName;
    if (EnumDisplayDevicesW(deviceNameW, 0, &displayDevice, EDD_GET_DEVICE_INTERFACE_NAME)) {
        monitorName = GetMonitorModelFromDeviceID(displayDevice.DeviceID);
    }

    if (monitorName.empty()) {
        // Try getting the name from registry path
        std::wstring deviceID = displayDevice.DeviceID;
        std::wstring monitorIDW;
        size_t startPos = deviceID.find(L"MONITOR\\");
        if (startPos != std::wstring::npos) {
            size_t endPos = deviceID.find(L"\\", startPos + 8);
            if (endPos != std::wstring::npos) {
                monitorIDW = deviceID.substr(startPos + 8, endPos - (startPos + 8));
                std::string monitorID(monitorIDW.begin(), monitorIDW.end());
                std::string manufacturer = GetManufacturerName(monitorID.substr(0, 3));
                if (!manufacturer.empty()) {
                    monitorName = manufacturer + " " + monitorID.substr(3);
                }
            }
        }
    }

    if (monitorName.empty()) {
        monitorName = "Unknown Monitor";
    }

    std::cout << "Monitor " << monitor.index << " name: " << monitorName << std::endl;
    monitor.name = monitorName;
    pMonitors->push_back(monitor);
    return TRUE;
}

std::vector<WindowsMonitor> WindowsUtils::getAllMonitors()
{
    std::vector<WindowsMonitor> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

} // namespace ScreenPlay