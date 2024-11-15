// networkinterface_win.cpp
#include "networkinterface_win.h"

#include <QtSystemDetection>
#ifdef Q_OS_WIN
namespace NetworkInterfaceImpl {
bool getInterfaceStats(NET_IFINDEX ifIndex, ULONG64& bytesReceived, ULONG64& bytesSent, bool& isActive)
{
    if (ifIndex == 0) {
        return false;
    }

    MIB_IF_ROW2 ifRow = {};
    ifRow.InterfaceIndex = ifIndex;

    if (GetIfEntry2(&ifRow) == NO_ERROR) {
        bytesReceived = ifRow.InOctets;
        bytesSent = ifRow.OutOctets;
        isActive = (ifRow.OperStatus == IfOperStatusUp);
        return true;
    }

    return false;
}
}
#endif
