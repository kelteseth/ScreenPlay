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
    const char* vectors that was duplicated across multiple files (and was
    buggy in steamapiwrapper.cpp where .toUtf8() temporaries dangled).

    Usage:
    \code
    SteamTagArray tags(myStringList);
    if (!tags.isEmpty())
        SteamUGC()->SetItemTags(updateHandle, tags.get());
    \endcode
*/
class SteamTagArray {
public:
    explicit SteamTagArray(const QStringList& tags, int maxTagLength = 255)
    {
        for (const auto& tag : tags) {
            if (tag.isEmpty())
                continue;
            if (tag.length() > maxTagLength) {
                qWarning() << "SteamTagArray: skipping tag exceeding max length" << maxTagLength << ":" << tag;
                continue;
            }
            m_storage.append(tag.toUtf8());
            m_pointers.append(m_storage.last().constData());
        }
        m_array.m_ppStrings = m_pointers.data();
        m_array.m_nNumStrings = m_pointers.size();
    }

    /// Returns a pointer to the SteamParamStringArray_t.  Valid as long as
    /// this SteamTagArray instance is alive.
    const SteamParamStringArray_t* get() const { return &m_array; }

    bool isEmpty() const { return m_pointers.isEmpty(); }
    int count() const { return m_pointers.size(); }

private:
    QVector<QByteArray> m_storage;
    QVector<const char*> m_pointers;
    SteamParamStringArray_t m_array {};
};

} // namespace ScreenPlayWorkshop
