#pragma once
#ifdef Q_OS_WIN
#include <qt_windows.h>

#include <QObject>

uint64_t FileTimeToInt64( const FILETIME& ft ) {
    ULARGE_INTEGER uli = {  };
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}
#endif
