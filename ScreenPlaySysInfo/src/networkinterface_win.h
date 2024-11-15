// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QtSystemDetection>

#ifdef Q_OS_WIN
// Windows version defines need to come before any Windows headers
#ifndef WINVER
#define WINVER 0x0601
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x06010000
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

// Include order is important for Windows headers
#include <sdkddkver.h>

#include <winsock2.h>

#include <ws2def.h>

#include <ws2ipdef.h>

#include <windows.h>

#include <ws2tcpip.h>

#include <iphlpapi.h>

#include <netioapi.h>

namespace NetworkInterfaceImpl {
bool getInterfaceStats(NET_IFINDEX ifIndex, ULONG64& bytesReceived, ULONG64& bytesSent, bool& isActive);
}
#endif
