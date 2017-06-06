#include "wallpaper.h"

BOOL WINAPI SearchForWorkerWindow(HWND hwnd, LPARAM lparam)
{
    // 0xXXXXXXX "" WorkerW
    //   ...
    //   0xXXXXXXX "" SHELLDLL_DefView
    //     0xXXXXXXXX "FolderView" SysListView32
    // 0xXXXXXXXX "" WorkerW                           <---- We want this one
    // 0xXXXXXXXX "Program Manager" Progman
    if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr))
        *reinterpret_cast<HWND*>(lparam) = FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
    return TRUE;
}

Wallpaper::Wallpaper(QWindow* parent)
    : QWindow(parent)
{
}

Wallpaper::Wallpaper(QRect rect, float playback, bool isLooping, int fillmode)
{
    m_rect = rect;
    m_playback = playback;
    m_isLooping = isLooping;
    m_fillmode = fillmode;
}
