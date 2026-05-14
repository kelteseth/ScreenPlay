// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QByteArray>
#include <QDebug>
#include <QStringList>
#include <QVector>

#include "steam/steam_api.h"

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamTagArray
    \inmodule ScreenPlayWorkshop
    \brief RAII helper that converts a QStringList into a SteamParamStringArray_t
           while keeping the underlying QByteArray storage alive.

    This replaces the error-prone pattern of manually managing QByteArray +
    const char* vectors that was duplicated across multiple files.

    Usage:
    \code
    SteamTagArray tags(myStringList);
    if (!tags.isEmpty())
        SteamUGC()->SetItemTags(updateHandle, tags.get());
    \endcode
*/
class SteamTagArray {
public:
    // The SDK has no per-tag byte limit; the only documented bound is
    // k_cchTagListMax (the comma-joined list buffer). A single tag can
    // therefore not exceed that size minus one byte for the trailing NUL.
    explicit SteamTagArray(const QStringList& tags, int maxTagBytes = k_cchTagListMax - 1)
    {
        // Note: Steam silently drops tags it considers invalid (emoji,
        // most punctuation, commas, ampersands, leading/trailing whitespace,
        // anything outside ASCII alphanumerics + space/_/-). The call still
        // succeeds — the offending tags just never appear on the item.
        m_storage.reserve(tags.size());
        m_pointers.reserve(tags.size());

        for (const auto& tag : tags) {
            if (tag.isEmpty())
                continue;
            QByteArray utf8 = tag.toUtf8();
            if (utf8.size() > maxTagBytes) {
                qWarning() << "SteamTagArray: skipping tag exceeding max length" << maxTagBytes << "bytes:" << tag;
                continue;
            }
            m_storage.append(std::move(utf8));
            m_pointers.append(m_storage.last().constData());
        }
        m_array.m_ppStrings = m_pointers.data();
        m_array.m_nNumStrings = static_cast<int32>(m_pointers.size());
    }

    /// Returns a pointer to the SteamParamStringArray_t.  Valid as long as
    /// this SteamTagArray instance is alive.
    const SteamParamStringArray_t* get() const { return &m_array; }

    bool isEmpty() const { return m_pointers.isEmpty(); }
    int count() const { return m_pointers.size(); }

    SteamTagArray(const SteamTagArray&) = delete;
    SteamTagArray& operator=(const SteamTagArray&) = delete;
    SteamTagArray(SteamTagArray&&) = delete;
    SteamTagArray& operator=(SteamTagArray&&) = delete;

private:
    QVector<QByteArray> m_storage;
    QVector<const char*> m_pointers;
    SteamParamStringArray_t m_array {};
};

} // namespace ScreenPlayWorkshop
