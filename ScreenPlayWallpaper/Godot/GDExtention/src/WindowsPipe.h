// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include "godot_cpp/variant/utility_functions.hpp"
#include <iostream>
#include <string>
#include <windows.h>

class WindowsPipe {
public:
    WindowsPipe()
        : m_hPipe(INVALID_HANDLE_VALUE)
    {
        memset(&m_overlappedRead, 0, sizeof(m_overlappedRead));
        memset(&m_overlappedWrite, 0, sizeof(m_overlappedWrite));
        m_overlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event for read
        m_overlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual reset event for write
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
            m_hPipe = INVALID_HANDLE_VALUE;
        }
    }
    ~WindowsPipe()
    {
        close();
        if (m_overlappedRead.hEvent) {
            CloseHandle(m_overlappedRead.hEvent);
        }
        if (m_overlappedWrite.hEvent) {
            CloseHandle(m_overlappedWrite.hEvent);
        }
    }

private:
    bool connectToPipe();

private:
    HANDLE m_hPipe;
    OVERLAPPED m_overlappedRead;
    OVERLAPPED m_overlappedWrite;
    std::wstring m_pipeName;
    bool m_readPending = false;
    char m_readBuffer[4096] = {};
};
