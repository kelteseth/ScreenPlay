#pragma once

#include <QDebug>
#include <QProcess>
#include <optional>

#if defined(Q_OS_WIN)
#include <Windows.h>
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif
namespace ScreenPlay {

class ProcessManager {
public:
    std::optional<bool> isRunning(const qint64 pid) const;
    bool terminateProcess(const qint64 pid) const;
    const qint64 getCurrentPID() const;
};
}
