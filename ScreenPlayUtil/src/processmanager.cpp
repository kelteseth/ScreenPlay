#include "processmanager.h"
namespace ScreenPlay {

std::optional<bool> ProcessManager::isRunning(const qint64 pid)
{
    if (pid <= 0)
        return std::nullopt;

#if defined(Q_OS_WIN)
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process) {
        DWORD exitCode;
        if (GetExitCodeProcess(process, &exitCode)) {
            CloseHandle(process);
            return exitCode == STILL_ACTIVE;
        }
        CloseHandle(process);
    }
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    int status = kill(pid, 0);
    if (status == 0)
        return true;
    if (errno == ESRCH)
        return false;
#endif
    return std::nullopt;
}
bool ProcessManager::terminateProcess(const qint64 pid)
{
    if (pid <= 0)
        return false;

#if defined(Q_OS_WIN)
    HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (process) {
        BOOL result = TerminateProcess(process, 1);
        CloseHandle(process);
        if (!result) {
            qDebug() << "Failed to terminate process on Windows. Error code:" << GetLastError();
            return false;
        }
        return true;
    } else {
        qDebug() << "Failed to open process for termination on Windows. Error code:" << GetLastError();
        return false;
    }
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    if (kill(pid, SIGTERM) == -1) {
        qDebug() << "Failed to send termination signal on Linux/macOS. Error:" << strerror(errno);
        return false;
    }
    return true;
#endif
}

}
