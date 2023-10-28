#pragma once

#include <iostream>
#include <string>
#include <windows.h>

class WindowsPipe {
public:
    WindowsPipe()
        : m_hPipe(INVALID_HANDLE_VALUE)
    {
        memset(&m_overlapped, 0, sizeof(m_overlapped));
        m_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event
    }

    void setPipeName(const std::wstring& pipeName)
    {
        m_pipeName = pipeName;
    }

    bool start()
    {
        return connectToPipe();
    }

    bool readFromPipe(std::string& outMessage);
    bool writeToPipe(const std::string& message);
    void close()
    {
        if (m_hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(m_hPipe);
        }
    }
    ~WindowsPipe()
    {
        close();
        CloseHandle(m_overlapped.hEvent);
    }

private:
    bool connectToPipe();

private:
    HANDLE m_hPipe;
    OVERLAPPED m_overlapped;
    std::wstring m_pipeName;
};
