// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "WindowsPipe.h"

bool WindowsPipe::connectToPipe()
{
    if (m_pipeName.empty()) {
        godot::UtilityFunctions::print("Pipe name not set.");
        return false;
    }


    std::wstring fullPipeName = L"\\\\.\\pipe\\" + m_pipeName;

    // Retry connection up to 10 times with 500ms delay
    for (int i = 0; i < 10; i++) {
        m_hPipe = CreateFileW(
            fullPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL);

        if (m_hPipe != INVALID_HANDLE_VALUE) {
            godot::UtilityFunctions::print("Connected to pipe successfully");
            return true;
        }

        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            godot::UtilityFunctions::print("Waiting for pipe server... (", i + 1, "/10)");
            Sleep(500);
        } else {
            godot::UtilityFunctions::print("Failed to connect to pipe. Error code: ", (int)error);
            return false;
        }
    }

    godot::UtilityFunctions::print("Failed to connect to pipe after 10 attempts");
    return false;
}

/*!
    \brief Reads from the named pipe using non-blocking overlapped I/O.
    
    This function uses a persistent pending read pattern. We track whether a read
    operation is already in progress (m_readPending) to avoid starting a new ReadFile
    call each poll, which would cancel the previous pending operation and lose data.
    
    The read buffer (m_readBuffer) must persist across calls since the data arrives
    asynchronously after ReadFile returns ERROR_IO_PENDING.
    
    \return true if a complete message was read, false otherwise (no data or pending)
*/
bool WindowsPipe::readFromPipe(std::string& outMessage)
{
    DWORD bytesRead = 0;

    // Check if there's a pending read operation first
    if (m_readPending) {
        // Check if the pending read has completed
        if (GetOverlappedResult(m_hPipe, &m_overlappedRead, &bytesRead, FALSE)) {
            m_readPending = false;
            if (bytesRead > 0) {
                m_readBuffer[bytesRead] = '\0';
                outMessage = std::string(m_readBuffer, bytesRead);
                godot::UtilityFunctions::print("Read from pipe: ", outMessage.c_str(), " (", (int)bytesRead, " bytes)");
                return true;
            }
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_IO_INCOMPLETE) {
                // Still pending, no data yet
                return false;
            }
            // Some other error occurred
            godot::UtilityFunctions::print("GetOverlappedResult failed. Error: ", (int)error);
            m_readPending = false;
            return false;
        }
    }

    // Start a new read operation
    ResetEvent(m_overlappedRead.hEvent);
    BOOL success = ReadFile(m_hPipe, m_readBuffer, sizeof(m_readBuffer) - 1, &bytesRead, &m_overlappedRead);

    if (success) {
        // Data was immediately available
        if (bytesRead > 0) {
            m_readBuffer[bytesRead] = '\0';
            outMessage = std::string(m_readBuffer, bytesRead);
            godot::UtilityFunctions::print("Read from pipe (immediate): ", outMessage.c_str(), " (", (int)bytesRead, " bytes)");
            return true;
        }
        return false;
    }

    DWORD error = GetLastError();
    if (error == ERROR_IO_PENDING) {
        // Read is pending, will check on next call
        m_readPending = true;
        return false;
    }

    godot::UtilityFunctions::print("ReadFile failed. Error: ", (int)error);
    return false;
}

bool WindowsPipe::writeToPipe(const std::string& message)
{
    DWORD bytesWritten;

    // Reset the event before starting a new write
    ResetEvent(m_overlappedWrite.hEvent);

    if (!WriteFile(m_hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, &m_overlappedWrite)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            godot::UtilityFunctions::print("WriteFile to pipe failed. Error code: ", (int)GetLastError());
            return false;
        }
        WaitForSingleObject(m_overlappedWrite.hEvent, INFINITE);
        if (!GetOverlappedResult(m_hPipe, &m_overlappedWrite, &bytesWritten, FALSE)) {
            godot::UtilityFunctions::print("Overlapped WriteFile failed. Error code: ", (int)GetLastError());
            return false;
        }
    }

    if (static_cast<size_t>(bytesWritten) != message.size()) {
        godot::UtilityFunctions::print("Bytes written mismatch.");
        return false;
    }

    return true;
}
