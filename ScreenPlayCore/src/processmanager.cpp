#include "processmanager.h"
namespace ScreenPlay {

std::optional<bool> ProcessManager::isRunning(const qint64 pid) const
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
    if (errno == EPERM)
        return true; // Process exists, but no permission to signal it
#endif
    return std::nullopt;
}
bool ProcessManager::terminateProcess(const qint64 pid) const
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

const qint64 ProcessManager::getCurrentPID() const
{
#if defined(Q_OS_WIN)
    return static_cast<qint64>(GetCurrentProcessId());
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    return static_cast<qint64>(getpid());
#endif
}

ProcessManager::ProcessState ProcessManager::getProcessState(const qint64 pid) const
{
    if (pid <= 0)
        return ProcessState::InvalidPID;

#if defined(Q_OS_WIN)
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process) {
        DWORD exitCode;
        if (GetExitCodeProcess(process, &exitCode)) {
            CloseHandle(process);
            return exitCode == STILL_ACTIVE ? ProcessState::Running : ProcessState::NotRunning;
        }
        CloseHandle(process);
    }
    return ProcessState::NotRunning; // Process doesn't exist or can't be accessed
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    int status = kill(pid, 0);
    if (status == 0)
        return ProcessState::Running;
    if (errno == ESRCH)
        return ProcessState::NotRunning; // Process doesn't exist
    if (errno == EPERM)
        return ProcessState::Running; // Process exists, but no permission to signal it
    return ProcessState::InvalidPID;
#endif
}
}
