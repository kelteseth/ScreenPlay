// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ScreenPlay {

/*!
    Accumulates raw socket bytes and extracts complete IPC frames.

    Local sockets are byte streams: writes from the peer can arrive coalesced
    into one readyRead (multiple frames per packet) or split across several
    (one frame per multiple packets). Parsing reads directly therefore drops
    or corrupts messages. Both ends of the ScreenPlay <-> wallpaper/widget
    connection feed their raw bytes through this buffer instead.

    Two frame kinds coexist on the wire:
    \list
    \li JSON objects ("{...}"), self-delimiting via brace matching. Senders
        can write them back-to-back without separators, and a ';' inside a
        JSON string (e.g. a file path) cannot break framing.
    \li Plain-text tokens terminated by ';' ("ping;", "appID=...,type;").
        This is the legacy framing also produced by the Godot GDExtension
        wallpaper, which is why it must keep working unchanged.
    \endlist

    Deliberately Qt-free (std:: types only): the same implementation is
    compiled into the Godot GDExtension wallpaper, which cannot link Qt, so
    every endpoint of the pipe shares identical framing code (target
    ScreenPlayCoreIpcLib). Qt callers convert via QByteArray::constData and
    QString::fromStdString (frames are UTF-8).
*/
class IpcFrameBuffer {
public:
    void append(std::string_view data);

    /*! Removes and returns all complete frames, in arrival order. JSON frames
        are returned including their braces; text frames are returned without
        the trailing ';' and trimmed. Incomplete trailing data stays buffered. */
    std::vector<std::string> takeFrames();

    std::size_t bufferedBytes() const { return m_buffer.size(); }
    void clear() { m_buffer.clear(); }

    /*! How many times the buffer was discarded because it exceeded
        maxBufferSize without a complete frame (corrupt stream). This class
        cannot log (no Qt); callers can watch this counter instead. */
    std::size_t overflowCount() const { return m_overflowCount; }

    // A frame larger than this cannot be legitimate; treat it as a corrupt
    // stream and drop the buffer instead of growing without bound.
    static constexpr std::size_t maxBufferSize = 1024 * 1024;

private:
    std::string m_buffer;
    std::size_t m_overflowCount = 0;
};

} // namespace ScreenPlay
