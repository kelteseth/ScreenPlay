// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayCore/ipcframebuffer.h"

namespace ScreenPlay {

namespace {
    // ASCII only: frame boundaries are all ASCII, and UTF-8 continuation
    // bytes have the high bit set so they can never collide.
    bool isInterFrameWhitespace(const char c)
    {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }
}

void IpcFrameBuffer::append(std::string_view data)
{
    m_buffer.append(data);
    if (m_buffer.size() > maxBufferSize) {
        // No logging here - this file must stay Qt-free for the Godot
        // GDExtension. Callers can watch overflowCount().
        ++m_overflowCount;
        m_buffer.clear();
    }
}

std::vector<std::string> IpcFrameBuffer::takeFrames()
{
    std::vector<std::string> frames;
    std::size_t pos = 0;
    const std::size_t size = m_buffer.size();

    while (pos < size) {
        const char c = m_buffer[pos];

        // Skip separators and stray whitespace between frames. Legacy senders
        // terminate JSON with ';' too - after the brace scan consumed the
        // object this swallows the leftover delimiter.
        if (c == ';' || isInterFrameWhitespace(c)) {
            ++pos;
            continue;
        }

        if (c == '{') {
            // JSON frame: find the matching closing brace. Braces and ';'
            // inside JSON strings must not count, so track string state and
            // backslash escapes.
            std::size_t end = std::string::npos;
            int depth = 0;
            bool inString = false;
            bool escaped = false;
            for (std::size_t i = pos; i < size; ++i) {
                const char ch = m_buffer[i];
                if (escaped) {
                    escaped = false;
                    continue;
                }
                if (ch == '\\') {
                    escaped = inString;
                    continue;
                }
                if (ch == '"') {
                    inString = !inString;
                    continue;
                }
                if (inString)
                    continue;
                if (ch == '{') {
                    ++depth;
                } else if (ch == '}') {
                    --depth;
                    if (depth == 0) {
                        end = i;
                        break;
                    }
                }
            }
            if (end == std::string::npos)
                break; // incomplete JSON - wait for more data

            frames.emplace_back(m_buffer.substr(pos, end - pos + 1));
            pos = end + 1;
            continue;
        }

        // Plain-text frame, terminated by ';'.
        const std::size_t sep = m_buffer.find(';', pos);
        if (sep == std::string::npos)
            break; // incomplete - wait for more data

        // Trim surrounding whitespace from the token.
        std::size_t begin = pos;
        std::size_t last = sep;
        while (begin < last && isInterFrameWhitespace(m_buffer[begin]))
            ++begin;
        while (last > begin && isInterFrameWhitespace(m_buffer[last - 1]))
            --last;
        if (last > begin)
            frames.emplace_back(m_buffer.substr(begin, last - begin));
        pos = sep + 1;
    }

    m_buffer.erase(0, pos);
    return frames;
}

} // namespace ScreenPlay
