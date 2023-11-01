#include "WindowsPipe.h"

bool WindowsPipe::connectToPipe()
{
    if (m_pipeName.empty()) {
        std::cerr << "Pipe name not set." << std::endl;
        return false;
    }

    std::wstring fullPipeName = L"\\\\.\\pipe\\" + m_pipeName;

    m_hPipe = CreateFileW(
        fullPipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL);

    if (m_hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to pipe. Error code: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

bool WindowsPipe::readFromPipe(std::string& outMessage)
{
    char buffer[128];
    DWORD bytesRead;

    if (!ReadFile(m_hPipe, buffer, sizeof(buffer) - 1, &bytesRead, &m_overlapped)) {
        DWORD error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            // Check if the overlapped read operation is complete without waiting
            if (!GetOverlappedResult(m_hPipe, &m_overlapped, &bytesRead, FALSE)) {
                if (GetLastError() == ERROR_IO_INCOMPLETE) {
                    // The I/O operation is still pending (no data available yet)
                    return false;
                }
                std::cerr << "Overlapped ReadFile failed. Error code: " << GetLastError() << std::endl;
                return false;
            }
        } else {
            std::cerr << "ReadFile from pipe failed. Error code: " << error << std::endl;
            return false;
        }
    }

    buffer[bytesRead] = '\0';
    outMessage = buffer;
    return true;
}

bool WindowsPipe::writeToPipe(const std::string& message)
{
    DWORD bytesWritten;

    if (!WriteFile(m_hPipe, message.c_str(), static_cast<DWORD>(message.size()), &bytesWritten, &m_overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            std::cerr << "WriteFile to pipe failed. Error code: " << GetLastError() << std::endl;
            return false;
        }
        WaitForSingleObject(m_overlapped.hEvent, INFINITE);
        if (!GetOverlappedResult(m_hPipe, &m_overlapped, &bytesWritten, FALSE)) {
            std::cerr << "Overlapped WriteFile failed. Error code: " << GetLastError() << std::endl;
            return false;
        }
    }

    if (static_cast<size_t>(bytesWritten) != message.size()) {
        std::cerr << "Bytes written mismatch." << std::endl;
        return false;
    }

    return true;
}
