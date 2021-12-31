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

#include <QtGlobal>

#if defined(Q_OS_WIN)
// Must be first!
#include <qt_windows.h>

#include "WinUser.h"
#include <ShellScalingApi.h>
#endif

#include "ScreenPlayUtil/contenttypes.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVersionNumber>
#include <optional>

namespace ScreenPlayUtil {
#if defined(Q_OS_WIN)
struct WinMonitorStats {

    WinMonitorStats()
    {
        EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)this);
    }

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor,
        LPARAM pData)
    {
        WinMonitorStats* pThis = reinterpret_cast<WinMonitorStats*>(pData);
        auto scaleFactor = DEVICE_SCALE_FACTOR::DEVICE_SCALE_FACTOR_INVALID;
        GetScaleFactorForMonitor(hMon, &scaleFactor);

        UINT x = 0;
        UINT y = 0;
        GetDpiForMonitor(hMon, MONITOR_DPI_TYPE::MDT_RAW_DPI, &x, &y);
        pThis->sizes.push_back({ x, y });
        pThis->scaleFactor.push_back(scaleFactor);
        pThis->hMonitors.push_back(hMon);
        pThis->hdcMonitors.push_back(hdc);
        pThis->rcMonitors.push_back(*lprcMonitor);
        pThis->iMonitors.push_back(pThis->hdcMonitors.size());

        //qInfo() << std::abs(lprcMonitor->right - lprcMonitor->left) << std::abs(lprcMonitor->top - lprcMonitor->bottom);

        return TRUE;
    }

    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitors;
    std::vector<HDC> hdcMonitors;
    std::vector<RECT> rcMonitors;
    std::vector<DEVICE_SCALE_FACTOR> scaleFactor;
    std::vector<std::pair<UINT, UINT>> sizes;
    int index = 0;
};
#endif
QJsonArray fillArray(const QVector<QString>& items);
ScreenPlay::SearchType::SearchType getSearchTypeFromInstalledType(const ScreenPlay::InstalledType::InstalledType type);
std::optional<ScreenPlay::InstalledType::InstalledType> getInstalledTypeFromString(const QString& type);
std::optional<ScreenPlay::VideoCodec::VideoCodec> getVideoCodecFromString(const QString& type);
std::optional<QJsonObject> parseQByteArrayToQJsonObject(const QByteArray& byteArray);
std::optional<QJsonObject> openJsonFileToObject(const QString& path);
std::optional<QString> openJsonFileToString(const QString& path);
std::optional<QVersionNumber> getVersionNumberFromString(const QString& str);
bool writeJsonObjectToFile(const QString& absoluteFilePath, const QJsonObject& object, bool truncate = true);
bool writeSettings(const QJsonObject& obj, const QString& absolutePath);
bool writeFile(const QString& text, const QString& absolutePath);
bool writeFileFromQrc(const QString& qrcPath, const QString& absolutePath);
bool copyPreviewThumbnail(QJsonObject& obj, const QString& previewThumbnail, const QString& destination);
QString toString(const QStringList& list);
QString toLocal(const QString& url);
QString generateRandomString(quint32 length = 32);
QString executableAppEnding();
QString executableBinEnding();
QStringList getAvailableWallpaper();
QStringList getAvailableWidgets();
QStringList getAvailableTypes();
QStringList getAvailableFillModes();
bool isWallpaper(const ScreenPlay::InstalledType::InstalledType type);
bool isWidget(const ScreenPlay::InstalledType::InstalledType type);
std::optional<QVector<int>> parseStringToIntegerList(const QString string);
}
